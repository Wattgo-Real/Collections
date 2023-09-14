import cv2 
import os 
import numpy as np
import time
import matplotlib.pyplot as plt
import random

import torch
import torch.nn as nn
from torch.nn import functional as F  
from torch import optim
from torchsummaryX import summary

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

def pad_zero(seqs, max_len):
    padded = np.full((len(seqs), max_len), fill_value=PAD_ID, dtype=np.int64)
    for i, seq in enumerate(seqs):
        padded[i, :len(seq)] = seq
    return padded

class MultiHead(nn.Module):
    def __init__(self, n_head, model_dim, drop_rate):
        super().__init__()
        self.head_dim = model_dim // n_head
        self.n_head = n_head
        self.model_dim = model_dim

        self.wq = nn.Linear(n_head * self.head_dim, n_head * self.head_dim)
        self.wk = nn.Linear(n_head * self.head_dim, n_head * self.head_dim)
        self.wv = nn.Linear(n_head * self.head_dim, n_head * self.head_dim)

        self.o_dense = nn.Linear(model_dim,model_dim)
        self.o_drop = nn.Dropout(drop_rate)
        self.attention = None

    def forward(self, q, k, v, mask):
        _q = self.wq(q)
        _k, _v = self.wk(k), self.wv(v) 
        _q = self.split_heads(_q)
        _k, _v = self.split_heads(_k), self.split_heads(_v)
        context = self.scaled_dot_product_attention(_q, _k, _v, mask)
        o = self.o_dense(context)
        o = self.o_drop(o)
        return o

    def split_heads(self, x):
        x = x.view(x.shape[0], x.shape[1], self.n_head, self.head_dim)
        return torch.transpose(x,1,2)

    def scaled_dot_product_attention(self, q, k, v, mask=None):
        k = torch.transpose(k,2,3)
        score = torch.matmul(q, k)
        if mask is not None:
            score += mask * -1e9
        self.attention = torch.softmax(score / ((self.model_dim) ** (1 / 2)), dim=-1)
        context = torch.matmul(self.attention, v)
        context = torch.transpose(context, 1,2)
        cs = context.shape
        context = context.contiguous().view(cs[0], cs[1], cs[2]*cs[3])

        return context
    
class TransformerBlock(nn.Module):
    def __init__(self, embed_size, heads, dropout, forward_expansion):
        super(TransformerBlock, self).__init__()
        self.attention = MultiHead(heads, embed_size, dropout)
        self.norm1 = nn.LayerNorm(embed_size)
        self.norm2 = nn.LayerNorm(embed_size)

        self.feed_forward = nn.Sequential(
            nn.Linear(embed_size, forward_expansion * embed_size),
            nn.ReLU(),
            nn.Linear(forward_expansion * embed_size, embed_size),
        )

        self.dropout = nn.Dropout(dropout)

    def forward(self, query, key, value, mask):
        attention = self.attention(query, key, value, mask)
        x = self.dropout(self.norm1(attention + query))
        forward = self.feed_forward(x)
        out = self.dropout(self.norm2(forward + x))
        return out

class DecoderBlock(nn.Module):
    def __init__(self, embed_size, heads, forward_expansion, dropout, device):
        super(DecoderBlock, self).__init__()
        self.norm = nn.LayerNorm(embed_size)
        self.attention = MultiHead(heads, embed_size, dropout)
        self.transformer_block = TransformerBlock(
            embed_size, heads, dropout, forward_expansion
        )
        self.dropout = nn.Dropout(dropout)

    def forward(self, x, value, key, trg_mask, src_mask):
        attention = self.attention(x, x, x, trg_mask)
        query = self.dropout(self.norm(attention + x))
        out = self.transformer_block(query, key, value, src_mask)
        return out
    
class Encoder(nn.Module):
    def __init__(self,src_vocab_size, embed_size, num_layers, heads, forward_expansion, 
                 dropout, device):
        super(Encoder, self).__init__()
        self.device = device

        self.layers = nn.ModuleList([TransformerBlock(embed_size,heads,dropout=dropout,forward_expansion=forward_expansion,)
             for _ in range(num_layers)])

    def forward(self, x, mask = None):
        for layer in self.layers:
            out = layer(x, x, x, mask)
	
        return out
    
class Decoder(nn.Module):
    def __init__(self, trg_vocab_size, embed_size, num_layers, heads, forward_expansion, 
                 dropout, device):
        super(Decoder, self).__init__()
        self.device = device
        self.layers = nn.ModuleList([DecoderBlock(embed_size, heads, forward_expansion, dropout, device)
                                     for _ in range(num_layers)])
        
        self.fc_out1 = nn.Linear(embed_size, embed_size)
        self.RelU = nn.ReLU(inplace=True)
        self.fc_out2 = nn.Linear(embed_size, trg_vocab_size)
        self.dropout = nn.Dropout(dropout)

    def forward(self, x, enc_out, trg_mask, src_mask = None):
        for layer in self.layers:
            x = layer(x, enc_out, enc_out, trg_mask, src_mask)

        out = self.fc_out1(x)
        out = self.RelU(out)
        out = self.fc_out2(out)

        return out
    
class PositionEmbedding(nn.Module):
    def __init__(self, max_len, model_dim, n_vocab):
        super().__init__()
        pos = np.arange(max_len)[:, None]
        pe = pos / np.power(10000, 2. * np.arange(model_dim)[None, :] / model_dim)
        pe[:, 0::2] = np.sin(pe[:, 0::2])
        pe[:, 1::2] = np.cos(pe[:, 1::2])
        pe = pe[None, :, :]
        self.pe = torch.tensor(pe,device=device)
        self.embeddings = nn.Embedding(
            n_vocab, model_dim)

    def forward(self, x):
        x_embed = self.embeddings(x) + self.pe
        return x_embed

MODEL_DIM = 64
Y_MAX_LEN = 24
X_MAX_LEN = 23*2 
N_LAYER = 3
N_HEAD = 4
DROP_RATE = 0.05
PADDING_IDX = 0

PAD_ID = 0
WikiQA_list = "TextRecognition/Data/Word_List.txt"
WikiQA_train = "TextRecognition/Data/WikiQA-train.txt"
class TextData:
    def __init__(self,is_out = True):
        self.v2i = {"<PAD>":0,"<EOS>":1,"<GO>":2}
        ss = 3
        for line in open(WikiQA_train, 'r', encoding="utf-8").readlines():
            for s in line:
                if s not in self.v2i:
                    self.v2i[s] = ss
                    ss += 1 
        self.i2v = {i: v for v, i in self.v2i.items()}
        self.x, self.y = [], []
        if is_out:
            self.x, self.y = self.data_out()
        
        self.start_token = self.v2i["<GO>"]
        self.end_token = self.v2i["<EOS>"]
    def data_out(self):
        set_word = []
        x,y = [],[]
        print("TextData in "+WikiQA_train)
        print("Loading and Process...")
        if os.path.exists(WikiQA_list) == False:
            for line in open(WikiQA_train, 'r', encoding="utf-8").readlines():
                s = line.split(" ")
                for w in s:
                    if "\n" not in w and w != "":
                        w_s = w.split("\t")
                        for w_st in w_s:
                            if w_st != "" and w_st not in set_word and len(w_st)<=22:
                                set_word.append(w_st)
            to_fd = ""
            for i in set_word:
                to_fd += i+","
            fd = open(WikiQA_list, "w", encoding="utf-8")
            fd.write(to_fd)
        else:
            for line in open(WikiQA_list, 'r', encoding="utf-8").readlines():
                to_set_word = line.split(",")
                for i in to_set_word:
                    if "\n" not in i and "\t" not in i and i != "" and " " not in i:
                        set_word.append(i)
        print("Total world in text: %d" % len(set_word))
        print("Start convert to image")
        for word in set_word:
            img = np.zeros((24, 300, 3), np.uint8)
            text = word
            cv2.putText(img, text, (4, 16), cv2.FONT_HERSHEY_COMPLEX_SMALL,1, (255, 255, 255), 1, cv2.LINE_AA)
            #pic_list = []
            x.append(img[:,:,0]/255)
            
            #for i in range(2):
            #    for j in range(23):
            #        pic_list.append(img[i*12:(i+1)*12,j*12:(j+1)*12,0]/255)
            #x.append(pic_list)
            y.append([self.v2i["<GO>"]] + [self.v2i[i] for i in word] + [self.v2i["<EOS>"]] +
                          [self.v2i["<PAD>"] for i in range(Y_MAX_LEN-1-len(word))])
        #x = torch.tensor(x,device=device,dtype=torch.float32)
        x = np.array(x)
        y = torch.tensor(y,device=device,dtype=torch.int64)
        return x,y

    def sample(self, n=64):
        bi = np.random.randint(0, len(self.x), size=n)
        bx, by = self.x[bi], self.y[bi]
        decoder_len = np.full((len(bx),), by.shape[1] - 1, dtype=np.int32)
        bxt = []
        for k in range(n):
            r = random.randint(-4,4)
            bh = random.randint(0,abs(r))
            bw = random.randint(0,abs(r))
            bxs = bx[k].shape
            bxn = cv2.resize(bx[k],(int(bxs[1]*((bxs[0]+r)/bxs[0])),bxs[0]+r))  
            imgw = np.zeros((24, 300))

            if r >= 0:
                imgw = bxn[bh:24+bh,bw:300+bw]
            else:
                a = abs(r)-bh
                b = abs(r)-bw
                imgw[a:bxn.shape[0]+a,b:bxn.shape[1]+b] = bxn

            pic_list = []
            for i in range(2):
                for j in range(23):
                    pic_list.append(imgw[i*12:(i+1)*12,j*12:(j+1)*12])
            bxt.append(pic_list)
        bxt = torch.tensor(np.array(bxt),dtype=torch.float32).to(device)
        return bxt, by, decoder_len
    
    def all_d(self):
        bx, by = self.x, self.y
        decoder_len = np.full((len(bx),), by.shape[1] - 1, dtype=np.int32)
        return bx, by, decoder_len

    def idx2str(self, idx):
        x = []
        for i in idx:
            x.append(self.i2v[i])
            if i == self.end_token:
                break
        return "".join(x)
    def out_p_to(self,bx,by):
        s_place = len(by)
        for i in range(len(by)):
            if by[i] == 1:
                s_place = i
                break
        x = by[1:s_place].detach().cpu().numpy()
        y = bx[0:s_place-1].detach().cpu().numpy()
        return x,y
    
    @property
    def num_word(self):
        return len(self.v2i)
    
class Transformer(nn.Module):
    def __init__(self, model_dim, x_max_len, y_max_len, n_layer, n_head, n_vocab, dev, drop_rate=0.1, padding_idx=0):
        super().__init__()
        self.x_max_len = x_max_len
        self.y_max_len = y_max_len
        self.padding_idx = padding_idx
        self.model_dim = model_dim
        
        self.embed = PositionEmbedding(y_max_len, model_dim, n_vocab)
        self.pos_embed = nn.Parameter(torch.zeros(1, x_max_len, model_dim))                      
        self.fc = nn.Linear(144,model_dim)

        self.encoder = Encoder(n_vocab, model_dim, n_layer, n_head, model_dim*4, drop_rate, dev)
        self.decoder = Decoder(n_vocab, model_dim, n_layer, n_head, model_dim*4, drop_rate, dev)
        
    def forward(self, x, y):
        x_s = x.shape
        x_embed = self.fc(x.view(x_s[0]*x_s[1],144)).view(x_s[0],x_s[1],self.model_dim) + self.pos_embed
        y_embed = self.embed(y).type(torch.float32)
        #pad_mask_x = self._pad_mask(x)
        pad_mask_y = self._look_ahead_mask(y)
        
        enc_src = self.encoder(x_embed)
        decoded_z = self.decoder(y_embed, enc_src, pad_mask_y)
          
        return decoded_z

    def _pad_bool(self, seqs):
        return torch.eq(seqs,self.padding_idx)

    def _pad_mask(self, seqs):
        mask = self._pad_bool(seqs).type(dtype=torch.float32)
        return mask.view(mask.shape[0],1,1,mask.shape[1])

    def _look_ahead_mask(self, seqs):
        mask = torch.triu(torch.ones(self.y_max_len, self.y_max_len), diagonal=1).to(device)
        mask = torch.where(self._pad_bool(seqs).view(seqs.shape[0],1,1,seqs.shape[1]), 1, 
                           mask.view(1,1,mask.shape[0],mask.shape[1]))
        return mask

    def translate(self, x, v2i, i2v, device):
        with torch.no_grad():
            tgt = torch.tensor(pad_zero(np.array([[v2i["<GO>"], ] for _ in range(len(x))]), 
                                        self.y_max_len+1), device=device)
            tgti = 0
            x_s = x.shape
            x_embed = self.fc(x.view(x_s[0]*x_s[1],144)).view(x_s[0],x_s[1],self.model_dim) + self.pos_embed
            enc_src = self.encoder(x_embed)
            while True:
                y = tgt[:, :-1]
                y_embed = self.embed(y).type(torch.float32)
                pad_mask_y = self._look_ahead_mask(y)
                decoded_z = self.decoder(y_embed, enc_src, pad_mask_y)[:, tgti, :]
                idx = torch.max(decoded_z,1)[1]
                tgti += 1
                tgt[:, tgti] = idx
                if tgti >= self.y_max_len:
                    break
            return tgt

    @property
    def attentions(self):
        attentions = {
            "encoder": [l.mh.attention.numpy() for l in self.encoder.ls],
            "decoder": {
                "mh1": [l.mh[0].attention.numpy() for l in self.decoder.ls],
                "mh2": [l.mh[1].attention.numpy() for l in self.decoder.ls],
        }}
        return attentions
    
def train(model, data, optimizer, step):
    save_acc = []
    t0 = time.time()
    for t in range(step):
        batch_size = 64
        bx, by, seq_len = data.sample(batch_size)
        n_vocab = data.num_word
          
        logits = model(bx, by[:, :-1])
        targets = F.one_hot(by[:, 1:], n_vocab).type(dtype=torch.float32)

        loss_fn = nn.CrossEntropyLoss()
        loss = loss_fn(logits.contiguous().view(batch_size*Y_MAX_LEN,n_vocab), targets.contiguous().view(batch_size*Y_MAX_LEN,n_vocab))
        
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()

        if t % 100 == 0:
            bx_test, by_test, seq_len_test = data.sample(200)
            by_out = model.translate(bx_test, data.v2i, data.i2v ,device)
            
            too = 0
            wrong = []
            tar = []
            for i in range(len(bx_test)):
                if torch.equal(by_out[i],by_test[i]):
                    too += 1
                else:
                    tar,wrong = data.out_p_to(by_out[i,1:],by_test[i]) 
            t1 = time.time()
            save_acc.append(too/len(bx_test))
                            
            dt,dw = data.out_p_to(torch.max(logits[0],1)[1],by[0])
            print(
                "step: ", t,
                "| time: %.2f" % (t1 - t0),
                "| loss: %.4f" % loss.detach().cpu().numpy(),
                "| Acc: %.2f %%\n" % ((too/len(bx_test))*100),
                "| target: ", "".join([data.i2v[i] for i in dt]),
                "| inference: ", "".join([data.i2v[i] for i in dw]),
                "\n| test traget: ", "".join([data.i2v[i] for i in tar]),
                "| test inference: ", "".join([data.i2v[i] for i in wrong]),
            )
            t0 = t1
    return save_acc

dot1 = []
dot2 = []
dot_list = []
img = np.zeros((620,900))
img2 = np.zeros((620,900))
rectangle_mode = False
event_mode = 0
def show_xy(event,x,y,flags,param):
    global dot1, dot2, rectangle_mode, event_mode, img, img2, dot_list
    if flags == 1:
        if event == 0:
            img2 = img.copy()
            dot2 = [x, y]
            cv2.rectangle(img2, (dot1[0], dot1[1]), (dot2[0], dot2[1]), 1, 1)
            cv2.imshow('image', img2)
            event_mode += 1
        elif event_mode >= 10:
            event_mode = 0
            if abs(dot1[0]-dot2[0]) * abs(dot1[1]-dot2[1]) > 100:
                dot_list = [dot1,dot2]
                rectangle_mode = True
                cv2.rectangle(img, (dot1[0], dot1[1]), (dot2[0], dot2[1]), 1, 1)
                img2 = img
        if event == 1:
            event_mode = 0
            dot1 = [x, y]
            
def test(model, data, optimizer, device):
    global dot1, dot2, img, img2, dot_list, rectangle_mode
    bx_test, by_test, seq_len_test = data.sample(20)
    img = np.zeros((620,900))
    bx = bx_test.detach().cpu().numpy()
    for k in range(20):
        for i in range(2):
            for j in range(23):
                img[k*30+10+i*12:k*30+10+(i+1)*12,10+j*12:10+(j+1)*12] = bx[k][i*23+j]
    q = img.copy()
    img2 = img.copy()
    cv2.namedWindow('image')
    cv2.setMouseCallback('image', show_xy)
    while(1):
        cv2.imshow('image',img2)
        k = cv2.waitKey(1)
        if rectangle_mode == True:
            rectangle_mode = False
            xt = q[dot_list[0][1]:dot_list[1][1],dot_list[0][0]:dot_list[1][0]]
            xs = xt.shape
            xt = cv2.resize(xt, (int(24*float(xs[1])/float(xs[0])), 24), interpolation=cv2.INTER_AREA)
            s = torch.zeros((1,46,12,12)).to(device)
            #q[24+dot_list[0][1]+i][dot_list[0][0]+j] = xt[i][j]
            #img = q.copy()
            #img2 = q.copy()
            
            for i in range(len(xt[0])):
                for j in range(24):
                    s[0][int(i/12)+23*int(j/12)][j%12][i%12] = xt[j][i]

            t = 0
            by_out = model.translate(s, data.v2i, data.i2v ,device)
            for i in range(len(by_out[0])):
                if(by_out[0][i] == 1):
                    t = i
                    break
            if t != 1:
                wrong = by_out[0,1:t].detach().cpu().numpy()
                outw = "".join([data.i2v[i] for i in wrong])
                imgq = np.zeros((24, 300, 3), np.uint8)
                cv2.putText(imgq, outw, (4, 16), cv2.FONT_HERSHEY_COMPLEX_SMALL,1, (255, 255, 255), 1, cv2.LINE_AA)

                q[dot_list[0][1]:dot_list[0][1]+24,dot_list[1][0]:dot_list[1][0]+300] = imgq[:,:,0]/255

                img = q.copy()
                img2 = q.copy()
                print(outw)
            else:
                print("Empty")
        if k==ord('e'):
            break

    '''
    bx_test, by_test, seq_len_test = data.sample(1)
    by_out = model.translate(bx_test, data.v2i, data.i2v ,device)
    tar,wrong = data.out_p_to(by_out[0,1:],by_test[0]) 

    print(
        "| test traget: ", "".join([data.i2v[i] for i in tar]),
        "| test inference: ", "".join([data.i2v[i] for i in wrong]),
    )
    '''

model_PATH = "TextRecognition/Model/Text_R_In_Trandformer.pth"   

if __name__ == "__main__":
    print("-----Start load data-----")
    dataset = TextData(is_out = True)
    Text_R_In_Transformer = Transformer(MODEL_DIM, X_MAX_LEN, Y_MAX_LEN, N_LAYER, N_HEAD, dataset.num_word, device,DROP_RATE, PADDING_IDX)
    optimizer = optim.Adam(Text_R_In_Transformer.parameters(), lr=0.001)
    
    if os.path.isfile(model_PATH): 
        checkpoint = torch.load(model_PATH)
        Text_R_In_Transformer.load_state_dict(checkpoint['Text_R_In_Transformer'])
        optimizer.load_state_dict(checkpoint['optimizer'])
        for state in optimizer.state.values():
            for k, v in state.items():
                if torch.is_tensor(v):
                    state[k] = v.cuda()
        #dataset.x = checkpoint['data_x'].type(torch.float32).to(device)
        #dataset.y = checkpoint['data_y'].type(torch.int64).to(device)
    else:
        dataset = TextData()
    print("-----Finish load data-----")
            
    Text_R_In_Transformer.to(device)
    print("x_shape:",dataset.x.shape,"| y_shape:",dataset.y.shape)
        
    #summary(Text_R_In_Transformer,torch.zeros(64,46,12,12).to(device), torch.zeros(64,24).to(device).type(torch.int64))
    for i in range(20):
        #train(Text_R_In_Transformer, dataset, optimizer, 10)
        test(Text_R_In_Transformer, dataset, optimizer, device)
        '''
        torch.save({'Text_R_In_Transformer': Text_R_In_Transformer.state_dict(),
                    'optimizer' : optimizer.state_dict()},model_PATH)
        #            'data_x' : dataset.x,
        #            'data_y' : dataset.y
        
   
        plt.plot([i for i in range(len(save_acc))],save_acc)
        plt.grid()
        plt.show()
        '''



    
    
    
    
    
    