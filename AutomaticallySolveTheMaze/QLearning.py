import numpy as np
import random
import cv2
import math
import time
import matplotlib.pyplot as plt

import torch
import torch.nn as nn
from torch.nn import functional as F  
from torch import optim
from collections import namedtuple, deque
from torchsummary import summary

import os
os.environ["KMP_DUPLICATE_LIB_OK"]="TRUE"

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(device)

w_h = 9

'''
start_game_state = np.array([[[3,3,3,3,3,3,3,3,3],
                              [3,1,0,0,5,0,0,2,3],
                              [3,0,0,0,5,0,0,0,3],
                              [3,5,5,0,5,0,5,5,3],
                              [3,0,0,0,5,0,0,0,3],
                              [3,0,0,0,5,0,0,0,3],
                              [3,0,5,5,5,5,5,0,3],
                              [3,0,0,0,0,0,0,0,3],
                              [3,3,3,3,3,3,3,3,3]]])


start_game_state = np.array([[[3,3,3,3,3,3,3],
                       [3,1,0,5,0,2,3],
                       [3,0,0,5,0,5,3],
                       [3,0,0,5,0,0,3],
                       [3,0,0,5,0,0,3],
                       [3,0,0,0,0,0,3],
                       [3,3,3,3,3,3,3]]])

def restart():
    return np.copy(start_game_state), np.copy(start_player_position)
'''
Transition = namedtuple('Transition',
                        ('state', 'action', 'player' , 'player_s', 'next_state', 'reward'))

class ReplayMemory(object):
    def __init__(self, capacity, state_batch_size):
        self.S_memory = deque([], maxlen=state_batch_size) #暫存，經過分析之後會傳到memory
        self.memory = deque([], maxlen=capacity)
        self.TDerror = deque([], maxlen=capacity) #紀錄memory所得分數
        self.size = capacity
        self.s_sample = []

    def push(self, *args):
        self.S_memory.append(Transition(*args))
 
    def cover(self, TDerror, pls):
        sample = random.random()
        if(len(self.memory) == self.size and sample > 0.5):
            f_small_p = [0,100000]
            for i in range(100):
                q = random.randrange(0,len(self.memory))
                if f_small_p[1] > self.TDerror[q]:
                    f_small_p[0] = q
                    f_small_p[1] = self.TDerror[q]
            
            self.memory[f_small_p[0]] = self.S_memory[pls]
            self.TDerror[f_small_p[0]] = TDerror
            return
        self.memory.append(self.S_memory[pls])
        self.TDerror.append(TDerror)
        
    def change(self, TDerror):
        for i in range(len(self.s_sample)):
            self.TDerror[self.s_sample[i]] = TDerror[i]

    def sample(self, batch_size):
        sample = random.sample(range(len(self.memory)), batch_size)
        self.s_sample = sample
        out = [self.memory[sample[i]] for i in range(batch_size)]
        return out

    def __len__(self):
        return len(self.memory)
   
def is_State_Correct(start_game_state, S_state, S_player):
    l = np.array([[0,-1],[1,0],[0,1],[-1,0]])

    player = [1,1]
    start_game_state[0,1,1] = 0
    ta = 1
    lta = 0
    shortest_path = np.array([[1,1]])
    while 1:
        if ta >= 3:
            ta -= 4
        elif ta < -3:
            ta += 4

        if lta >= 3:
            lta -= 4
        elif lta < -3:
            lta += 4
        if start_game_state[0,player[0]+l[ta,0],player[1]+l[ta,1]] == -1:
            player += l[ta]
            shortest_path = np.append(shortest_path,[player],axis=0)
            start_game_state[0,1,1] = -3
            if len(S_state) == 0:
                S_state = start_game_state.copy()
                S_player.append([1,1])
            else:
                S_state = np.vstack([S_state,start_game_state.copy()])
                S_player.append([1,1])
                
            S_path_len = len(shortest_path)
            isP = False
            while isP == False:
                isP = True
                last_p = np.zeros((2))
                last_dis = np.zeros((2))
                last_t = 0
                save_j = 0
                for j in range(S_path_len):
                    if last_p[0] != 0:
                        dis = shortest_path[j] - last_p
                        if last_dis[0] != 0 or last_dis[1] != 0:
                            tur_t = last_dis[0]*dis[1] - last_dis[1]*dis[0]
                            if last_t != 0:
                                if last_t == 1 and tur_t == 1 or last_t == -1 and tur_t == -1:
                                    if j - save_j == 1:
                                        shortest_path[j-2:S_path_len-2] = shortest_path[j:S_path_len]
                                        shortest_path = shortest_path[0:S_path_len-2]
                                        S_path_len -= 2
                                        isP = False
                                        break
                                    elif j - save_j > 1:
                                        is_Wall = False
                                        nt = j-save_j-1
                                        for k in range(nt):
                                            if start_game_state[0,shortest_path[k+save_j,0]+dis[0],shortest_path[k+save_j,1]+dis[1]] == 3:
                                                is_Wall = True
                                        if is_Wall == False:
                                            be = True
                                            for k in range(nt):
                                                shortest_path[save_j-1+k] = [shortest_path[save_j+k,0]+dis[0], shortest_path[save_j+k,1]+dis[1]]
                                                for l in range(S_path_len-j):
                                                    if shortest_path[save_j-1+k,0] == shortest_path[j+l,0] and shortest_path[save_j-1+k,1] == shortest_path[j+l,1]:
                                                        be = False
                                                        shortest_path[save_j+k:save_j+k+S_path_len-j-l] = shortest_path[j+l:S_path_len]
                                                        S_path_len = save_j+k-j-l
                                                        shortest_path = shortest_path[0:S_path_len]
                                                        break
                                                if be == False:
                                                    break
                                            if be == True:
                                                shortest_path[j-2:S_path_len-2] = shortest_path[j:S_path_len]
                                                S_path_len -= 2
                                                shortest_path = shortest_path[0:S_path_len]
                                            isP = False
                                            break
                            if tur_t != 0:
                                last_t = tur_t
                                save_j = j
                        last_dis = dis
                    last_p = shortest_path[j]
            if shortest_path[0,0] == shortest_path[2,0] and shortest_path[0,1] == shortest_path[2,1]:
                shortest_path = shortest_path[2:len(shortest_path)]
            return S_state, S_player, True, shortest_path
        if player[0]+l[lta,0] == 0 and player[1]+l[lta,1] == 1:
            return S_state, S_player, False, shortest_path
        elif start_game_state[0,player[0]+l[lta,0],player[1]+l[lta,1]] == 0:
            if start_game_state[0,player[0]+l[lta,0],player[1]+l[lta,1]] != 3:
                ta -= 1
                lta -= 1
            player += l[ta]
            is_n = False
            if len(shortest_path) >= 2:
                for i in range(len(shortest_path)-2):
                    if shortest_path[len(shortest_path)-2-i][0] == player[0] and shortest_path[len(shortest_path)-2-i][1] == player[1]:
                        is_n = True
                        shortest_path = shortest_path[:-1-i]
                        break
            if is_n == False: 
                shortest_path = np.append(shortest_path,[player],axis=0)
        else:
            ta += 1
            lta += 1
    
def randomState(barrier,times):
    s_times = 0
    S_state = []
    S_player = []
    shortest_path = np.zeros((times,49,2))
    #print(barrier)
    if barrier > 20:
        barrier = 20
    while s_times < times: 
        S_path = []
        start_game_state = np.array([[[3,3,3,3,3,3,3,3,3],
                                [3,-3,0,0,0,0,0,0,3],
                                [3,0,0,0,0,0,0,0,3],
                                [3,0,0,0,0,0,0,0,3],
                                [3,0,0,0,0,0,0,0,3],
                                [3,0,0,0,0,0,0,0,3],
                                [3,0,0,0,0,0,0,0,3],
                                [3,0,0,0,0,0,0,-1,3],
                                [3,3,3,3,3,3,3,3,3]]])

        # set wall position
        for i in range(barrier):
            isS = False
            max_loop = 0
            while isS == False:
                max_loop += 1
                x = random.randrange(1,8)
                y = random.randrange(1,8)
                if start_game_state[0,x,y] != 3 and start_game_state[0,x,y] != -3 and start_game_state[0,x,y] != -1:
                    start_game_state[0,x,y] = 3

                    _, _ , isT , _ = is_State_Correct(start_game_state.copy(), S_state.copy(), S_player.copy())
    
                    if isT == True:
                        isS = True
                    else:
                        start_game_state[0,x,y] = 0
                if max_loop > 1000:
                    break
            if max_loop > 1000:
                break
        
        S_state, S_player, isT, S_path = is_State_Correct(start_game_state, S_state, S_player)

        if isT == True:
            shortest_path[s_times,0:len(S_path)] = S_path
            s_times += 1
        #time.sleep(100)    
    return S_state, np.array(S_player), shortest_path

def state_update(state, player, action):
    move_list_h = [-1,0,1,0]
    move_list_w = [0,1,0,-1]
    size = len(state)

    reward_list = np.zeros(size)
    isFalse_list = []
    for i in range(size):
        to_P = [player[i,0]+move_list_h[action[i]],
              player[i,1]+move_list_w[action[i]]]
        if state[i,to_P[0],to_P[1]] == 3 or state[i,to_P[0],to_P[1]] == 1:
            state[i,player[i,0],player[i,1]] = 3
            state[i,to_P[0],to_P[1]] = -3
            reward_list[i] = -1
            isFalse_list.append(True)
        elif state[i,to_P[0],to_P[1]] == -1:
            state[i,player[i,0],player[i,1]] = 3
            state[i,to_P[0],to_P[1]] = -3
            reward_list[i] = 1.
            isFalse_list.append(True)
        else:
            state[i,player[i,0],player[i,1]] = 3
            state[i,to_P[0],to_P[1]] = -3
            player[i] = [to_P[0],to_P[1]]
            reward_list[i] = 0.
            isFalse_list.append(False)

    return state, player, reward_list, isFalse_list,

class ResFc(nn.Module):
    def __init__(self, in_channels, out_channels):
        super(ResFc, self).__init__()
        self.fcM = nn.Sequential(
                nn.Linear(in_channels, in_channels),
                #nn.BatchNorm1d(in_channels),
                nn.ReLU(inplace=True),
                nn.Linear(in_channels, in_channels),
                #nn.BatchNorm1d(in_channels),
                nn.ReLU(inplace=True),
            )
        self.fc = nn.Linear(in_channels,out_channels)
    def forward(self,x):
        out = self.fcM(x)
        out = F.relu(self.fc(F.relu(x+out)))
        return out

class BN_Conv2d(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride, padding, dilation=1, groups=1, bias=False):
        super(BN_Conv2d, self).__init__()
        self.seq = nn.Sequential(
            nn.Conv2d(in_channels, out_channels, kernel_size=kernel_size, stride=stride,
                      padding=padding, dilation=dilation, groups=groups, bias=bias),
            nn.BatchNorm2d(out_channels)
        )

    def forward(self, x):
        return F.relu(self.seq(x))
    
class ResNeXt_Block(nn.Module):
    def __init__(self, in_chnls, cardinality, group_depth, stride):
        super(ResNeXt_Block, self).__init__()
        self.group_chnls = cardinality * group_depth
        self.conv1 = BN_Conv2d(in_chnls, self.group_chnls, 1, stride=1, padding=0)
        self.conv2 = BN_Conv2d(self.group_chnls, self.group_chnls, 3, stride=stride, padding=1, groups=cardinality)
        self.conv3 = nn.Conv2d(self.group_chnls, self.group_chnls*2, 1, stride=1, padding=0)
        self.bn = nn.BatchNorm2d(self.group_chnls*2)
        self.short_cut = nn.Sequential(
            nn.Conv2d(in_chnls, self.group_chnls*2, 1, stride, 0, bias=False),
            nn.BatchNorm2d(self.group_chnls*2)
        )

    def forward(self, x):
        out = self.conv1(x)
        out = self.conv2(out)
        out = self.bn(self.conv3(out))
        out += self.short_cut(x)
        return F.relu(out)
    
class ResConv(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size):
        super(ResConv, self).__init__()
        self.conv1 = nn.Sequential(
                nn.Conv2d(in_channels, in_channels/4, 1),
                nn.BatchNorm2d(out_channels),
                nn.ReLU(inplace=True),
                nn.Conv2d(in_channels/4, in_channels/4, 3, 1, 1),
                nn.BatchNorm2d(out_channels),
                nn.ReLU(inplace=True),
                nn.Conv2d(in_channels, in_channels/4, 1, 1),
                nn.BatchNorm2d(out_channels),
                nn.ReLU(inplace=True),
            )
        
        self.conv2 = nn.Conv2d(in_channels, out_channels, kernel_size)
        self.BNorm = nn.BatchNorm2d(out_channels)
        
        self.conv3 = nn.Sequential(
                nn.Conv2d(out_channels, out_channels, 3, 1, 1),
                nn.BatchNorm2d(out_channels),
                nn.ReLU(inplace=True),
                nn.Conv2d(out_channels, out_channels, 3, 1, 1),
                nn.BatchNorm2d(out_channels),
                nn.ReLU(inplace=True),
            )
    def forward(self,x):
        a = self.conv1(x)
        x = F.relu(self.BNorm(self.conv1(x)))
        b = self.conv3(x)
        x = x + b
        x = a + x
        return x

class DQN(nn.Module):
    def __init__(self, layers: object, cardinality, group_depth):
        super(DQN, self).__init__()
        self.cardinality = cardinality
        self.channels = 64
        self.conv1 = BN_Conv2d(1, self.channels, 3, stride=1, padding=0)
        self.conv2 = self.___make_layers(group_depth, layers[0], stride=1)
        self.conv3 = BN_Conv2d(self.channels, self.channels, 3, stride=1, padding=0)
        self.conv4 = self.___make_layers(group_depth*2, layers[1], stride=1)
        self.conv5 = BN_Conv2d(self.channels, self.channels, 3, stride=1, padding=0)
        self.conv6 = self.___make_layers(group_depth*4, layers[2], stride=1)
        
        self.conv1_V = nn.Conv2d(1024,128,3)  #3-1
        self.res1_V = ResFc(128,32)
        self.res2_V = ResFc(32,8)
        self.fc1_V = nn.Linear(8,1)

        self.conv1_A = nn.Conv2d(1024,128,3)  #2-1
        self.res1_A = ResFc(128,64)
        self.res2_A = ResFc(64,32)
        self.res3_A = ResFc(32,16)
        self.fc1_A = nn.Linear(16,4)
        
    def ___make_layers(self, d, blocks, stride):
        strides = [stride] + [1] * (blocks-1)
        layers = []
        for stride in strides:
            layers.append(ResNeXt_Block(self.channels, self.cardinality, d, stride))
            self.channels = self.cardinality*d*2
        return nn.Sequential(*layers)
    
    def forward(self,x,player):
        x = self.conv1(x)
        x = self.conv2(x)
        x = self.conv3(x)
        x = self.conv4(x)
        x = self.conv5(x)
        x = self.conv6(x)
                
        val = torch.flatten(F.relu(self.conv1_V(x)),1)
        val = self.res1_V(val)
        val = self.res2_V(val)
        val = self.fc1_V(val)
        
        adv = torch.flatten(F.relu(self.conv1_A(x)),1)
        adv = self.res1_A(adv)
        adv = self.res2_A(adv)
        adv = self.res3_A(adv)
        adv = self.fc1_A(adv)

        advAverage = torch.mean(adv, dim=1, keepdim=True)
        
        return val + adv - advAverage

STATE_BATCH_SIZE = 16
BATCH_SIZE = 32
MEMORY_SIZE = 3000
GAMMA = 0.998
EPS_START = 0.001
EPS_END = 0.001
EPS_DECAY = 1000
TAU = 0.01

SaveImg = [[] for i in range(STATE_BATCH_SIZE)]

accMemory = deque([], maxlen=100)

DQN_policy = DQN([3, 2, 2], 32, 4).to(device)
DQN_target = DQN([3, 2, 2], 32, 4).to(device)
DQN_target.load_state_dict(DQN_policy.state_dict())
memory = ReplayMemory(MEMORY_SIZE ,STATE_BATCH_SIZE)

policy_PATH = "AutomaticallySolveTheMaze\\Model\\DQN\\DQN_network.pth"
optimizer = optim.Adam(DQN_policy.parameters(), lr=0.0001)

good_S = 0.
if os.path.isfile(policy_PATH):
   print("The")
   checkpoint = torch.load(policy_PATH)
   DQN_policy.load_state_dict(checkpoint['DQN_policy_state_dict'])
   DQN_target.load_state_dict(checkpoint['DQN_target_state_dict'])
   good_S = checkpoint['Good_start']
   optimizer.load_state_dict(checkpoint['optimizer_state_dict']) 
   memory = checkpoint['Memory']

good = 0. + good_S

#summary(DQN_policy, (1, 9, 9))

steps_done = 0

def select_action(state, player):
    global steps_done
    size = len(state)
    
    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * math.exp(-1. * steps_done / EPS_DECAY)
    steps_done += 1
    if sample > eps_threshold:  #隨機選取機率
        with torch.no_grad():
            return DQN_policy(state, player).max(1)[1].view(size, 1)  #選擇值最高的那個
    else:
        ran = []
        for i in range(size):
            ran.append(random.randrange(4))
        return torch.tensor(ran,device=device).view(size, 1)
    
def optimize_model(re_times):
    Start = STATE_BATCH_SIZE-re_times
    transitions_p = [memory.S_memory[i] for i in range(Start, STATE_BATCH_SIZE)] #從剛剛存進去的讀取數值
    batch_p = Transition(*zip(*transitions_p))
    
    #取得有下一個狀態的位置
    next_state_mask = torch.tensor(tuple(map(lambda s: s is not None,batch_p.next_state)), device=device, dtype=torch.bool)
    
    next_state_st = [s for s in batch_p.next_state if s is not None]
    player_st = [s for s in batch_p.player_s if s is not None]
    #當剛剛存進去的數值沒有下一個狀態的話，就不紀錄
    next_state = [] if (len(next_state_st) == 0) else torch.cat(next_state_st)
    player_s = [] if (len(player_st) == 0) else torch.cat(player_st)

    state_batch = torch.cat(batch_p.state)
    player_batch = torch.cat(batch_p.player)
    action_batch = torch.cat(batch_p.action).type(torch.int64)
    reward_batch = torch.cat(batch_p.reward)

    #數據庫紀錄長度的超過了...次，新加的的與數據庫中紀錄的一起合併
    if len(memory) >= BATCH_SIZE * 1:
        transitions = memory.sample(BATCH_SIZE)
        batch = Transition(*zip(*transitions))
        
        next_state_mask_s = torch.tensor(tuple(map(lambda s: s is not None,
                                              batch.next_state)), device=device, dtype=torch.bool)

        next_state_sts = [s for s in batch.next_state if s is not None]
        player_s_sts = [s for s in batch.player_s if s is not None]
        isNone = len(next_state_sts) == 0
            
        #組合數據
        if len(next_state) != 0 and isNone == False:
            next_state = torch.cat([next_state, torch.cat(next_state_sts)], dim=0)
            player_s = torch.cat([player_s, torch.cat(player_s_sts)], dim=0)
        elif isNone == False:
            next_state = torch.cat(next_state_sts)
            player_s = torch.cat(player_s_sts)
            
        next_state_mask = torch.cat([next_state_mask, next_state_mask_s], dim=0)
        
        state_batch = torch.cat([state_batch, torch.cat(batch.state)], dim=0)
        player_batch = torch.cat([player_batch, torch.cat(batch.player)], dim=0)
        action_batch = torch.cat([action_batch, torch.cat(batch.action).type(torch.int64)], dim=0)
        reward_batch = torch.cat([reward_batch, torch.cat(batch.reward)], dim=0)
    
    total_len = len(action_batch)
    
    #公式是 Q_(s,a) - (r + GAMMA*Q_(s',a')) s' 為下一個狀態
    state_action_values = DQN_policy(state_batch,player_batch).gather(1, action_batch)
    
    #如果沒有下一個狀態，給0
    next_state_values = torch.zeros(total_len, device=device) 
    with torch.no_grad():
        if len(next_state) != 0:
        #    next_state_values[next_state_mask] = DQN_target(next_state).max(1)[0]
            next_action = DQN_policy(next_state,player_s).max(1)[1].view(len(next_state),1)
            next_state_values[next_state_mask] = DQN_target(next_state,player_s).gather(1, next_action).view(len(next_state))

    expected_state_action_values = reward_batch + (next_state_values * GAMMA)
    #得到錯誤分數，if這個狀態數值太小->不顯著，所以更容易被新加的替換
    TDerror = abs(state_action_values.squeeze(1) - expected_state_action_values)
    
    #替換數據庫紀錄的錯誤值
    memory.change(TDerror[re_times:total_len])
    #print(memory.TDerror)
    
    #將新加的錯誤值紀錄起來
    for i in range(re_times):
        memory.cover(TDerror[i],Start+i)
    #print( next_state_values)

    criterion = nn.SmoothL1Loss()
    loss = criterion(state_action_values, expected_state_action_values.unsqueeze(1))
    optimizer.zero_grad()
    loss.backward()
    torch.nn.utils.clip_grad_value_(DQN_policy.parameters(), 100)
    optimizer.step()
    
    #print(DQN_policy(state_batch))

#oop = 647;
for epoch in range(1):
    state, player, path = randomState(int(good),STATE_BATCH_SIZE)
    state = torch.tensor(state, dtype=torch.float32, device=device)
    Score_S = []
    for batch_idx in range(1000): 
        action = select_action(state.view(STATE_BATCH_SIZE,1,9,9)/2, player)
        
        next_state, player_s, reward, stop = state_update(state.clone(), player, action)

        reward = torch.tensor(reward, device=device).view(STATE_BATCH_SIZE,1)
        
        player_tensor = torch.tensor(player, dtype=torch.int32, device=device).view(STATE_BATCH_SIZE,2)
        player_s_tensor = torch.tensor(player_s, dtype=torch.int32, device=device).view(STATE_BATCH_SIZE,2)
        re_times = 0 
        
        for i in range(STATE_BATCH_SIZE):
            if stop[i]:
                memory.push(state[i].view(1,1,9,9)/2, action[i].view(1,1), player_tensor[i].view(1,2), None, 
                            None, reward[i].view(1))
                '''
                if reward[i] == 1:
                    print(len(SaveImg[i]))
                    if len(SaveImg[i]) >= 18:
                        for k in range(len(SaveImg[i])):
                            oop += 1
                            if oop > 999:
                                cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+str(oop)+".png",SaveImg[i][k])
                            elif oop > 99:
                                cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+"0"+str(oop)+".png",SaveImg[i][k])
                            elif oop > 9:
                                cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+"00"+str(oop)+".png",SaveImg[i][k])
                            else:
                                cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+"000"+str(oop)+".png",SaveImg[i][k])
                        pico = SaveImg[i][len(SaveImg[i])-1]
                        ba = int(400/w_h)
                        print(pico[7*ba-1,8*ba-1])
                        print(pico[8*ba-1,7*ba-1])
                        if pico[7*ba-1,8*ba-1,0] == 0 and pico[7*ba-1,8*ba-1,1] == 255:
                            pico[6*ba:7*ba,7*ba:8*ba] = [0,0,0]
                        elif pico[8*ba-1,7*ba-1,0] == 0 and pico[8*ba-1,7*ba-1,1] == 255:
                            pico[7*ba:8*ba,6*ba:7*ba] = [0,0,0]
                        pico[7*ba:8*ba,7*ba:8*ba] = [0,255,0]
                        oop += 1
                        if oop > 999:
                            cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+str(oop)+".png",pico)
                        elif oop > 99:
                            cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+"0"+str(oop)+".png",pico)
                        elif oop > 9:
                            cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+"00"+str(oop)+".png",pico)
                        else:
                            cv2.imwrite('C:\\Users\\m0987\\Desktop\\o\\'+"000"+str(oop)+".png",pico)

                    if oop > 1200:
                        break
                SaveImg[i] = []
                '''
            else:
                memory.push(state[i].view(1,1,9,9)/2, action[i].view(1,1), player_tensor[i].view(1,2), player_s_tensor[i].view(1,2),
                            next_state[i].view(1,1,9,9)/2, reward[i].view(1))
            re_times += 1

        state = next_state
        player = player_s
        
        #optimize_model(re_times)
        
        #DQN_target_state_dict = DQN_target.state_dict()
        #DQN_policy_state_dict = DQN_policy.state_dict()
        #for key in DQN_policy_state_dict:
        #    DQN_target_state_dict[key] = DQN_policy_state_dict[key]*TAU + DQN_target_state_dict[key]*(1-TAU)
        #DQN_target.load_state_dict(DQN_target_state_dict)

        #輸出畫面
        if (good >= 0):
            start_game_state = state
            k2 = 2
            ks = 400
            ksize = int(ks/k2)
            
            pic = np.full((ks,ks,3),(255,255,255),np.uint8)
            for p in range(k2):
                for p2 in range(k2):
                    ba = int(ksize/w_h)
                    for i in range(9):
                        for j in range(9):
                            for k in range(len(path[p+p2*2])):
                                if path[p+p2*k2,k,0] == i and path[p+p2*2,k,1] == j:
                                    pic[ksize*p+i*ba:ksize*p+(i+1)*ba,ksize*p2+j*ba:ksize*p2+(j+1)*ba] = [255,255,255]
                            if start_game_state[p+p2*k2,i,j] == -3:
                                pic[ksize*p+i*ba:ksize*p+(i+1)*ba,ksize*p2+j*ba:ksize*p2+(j+1)*ba] = [0,255,0]
                            elif start_game_state[p+p2*k2,i,j] == -1:
                                pic[ksize*p+i*ba:ksize*p+(i+1)*ba,ksize*p2+j*ba:ksize*p2+(j+1)*ba] = [100,200,0]
                            elif start_game_state[p+p2*k2,i,j] == 3:
                                pic[ksize*p+i*ba:ksize*p+(i+1)*ba,ksize*p2+j*ba:ksize*p2+(j+1)*ba] = [0,0,0]
            #SaveImg[lm].append(pic)
            cv2.imshow("maze",pic)

            cv2.waitKey(1) 
            
        for i in range(len(stop)):
            sg = good
            if stop[i]:
                state_t, player_t, path_t = randomState(int(good-random.randrange(int(good/2+1))),1)
                state[i] = torch.tensor(state_t, dtype=torch.float32, device=device)[0]
                player[i] = player_t
                path[i] = path_t
                if reward[i] >= 1 and good <= 35:
                    accMemory.append(1)
                    good += 0.1
                else:
                    accMemory.append(0)
                    good -= 0.1
                    if good < 0:
                        good = 0

        if((batch_idx+1)%100 == 0):  
            Score_S.append(good)
            
        if((batch_idx+1)%500 == 0):
            y = [i for i in range(len(Score_S))]
            plt.title('Block: %d' %epoch)
            plt.xlabel('Block')
            plt.ylabel('Epoth')
            plt.plot(y, Score_S, color='blue',linewidth=3)   
            plt.show()
    Score_S = []
    torch.save({'DQN_policy_state_dict': DQN_policy.state_dict(),
                'DQN_target_state_dict': DQN_target.state_dict(),
                'Good_start': good,
                'optimizer_state_dict' : optimizer.state_dict(),
                'Memory' : memory},policy_PATH)



