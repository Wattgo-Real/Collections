
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <time.h>
#include <string>
#include <vector>
#include "opencv2/calib3d.hpp"
#include <stdlib.h>
#include <math.h>

#define RATIO    0.4

using namespace cv;
using namespace std;

char k_size_3[16][2] = {
    {0, 3}, {1, 3}, {2, 2}, {3, 1},
    {3, 0}, {3, -1}, {2, -2}, {1, -3},
    {0, -3}, {-1, -3}, {-2, -2}, {-3, -1},
    {-3, 0}, {-3, 1}, {-2, 2}, {-1, 3}, 
};

static int bit_pattern_31_[256 * 4] = { 8,-3, 9,5/*mean (0), correlation (0)*/,
    4,2, 7,-12/*mean (1.12461e-05), correlation (0.0437584)*/,
    -11,9, -8,2/*mean (3.37382e-05), correlation (0.0617409)*/,
    7,-12, 5,-13/*mean (5.62303e-05), correlation (0.0636977)*/,
    2,-13, 2,12/*mean (0.000134953), correlation (0.085099)*/,
    1,-7, 1,6/*mean (0.000528565), correlation (0.0857175)*/,
    -2,-10, -2,-4/*mean (0.0188821), correlation (0.0985774)*/,
    -9,-7, -11,-8/*mean (0.0363135), correlation (0.0899616)*/,
    -13,-3, -12,-9/*mean (0.121806), correlation (0.099849)*/,
    10,4, 11,9/*mean (0.122065), correlation (0.093285)*/,
    -13,-8, -8,-9/*mean (0.162787), correlation (0.0942748)*/,
    -11,7, -9,12/*mean (0.21561), correlation (0.0974438)*/,
    7,7, 12,6/*mean (0.160583), correlation (0.130064)*/,
    -4,-5, -3,0/*mean (0.228171), correlation (0.132998)*/,
    -13,2, -12,-3/*mean (0.00997526), correlation (0.145926)*/,
    -9,0, -7,5/*mean (0.198234), correlation (0.143636)*/,
    12,-6, 12,-1/*mean (0.0676226), correlation (0.16689)*/,
    -3,6, -2,12/*mean (0.166847), correlation (0.171682)*/,
    -6,-13, -4,-8/*mean (0.101215), correlation (0.179716)*/,
    9,-11, 12,-8/*mean (0.200641), correlation (0.192279)*/,
    4,7, 5,1/*mean (0.205106), correlation (0.186848)*/,
    5,-3, 10,-3/*mean (0.234908), correlation (0.192319)*/,
    3,-7, 6,12/*mean (0.0709964), correlation (0.210872)*/,
    -8,-7, -6,-2/*mean (0.0939834), correlation (0.212589)*/,
    -2,11, -1,-10/*mean (0.127778), correlation (0.20866)*/,
    -7,6, -8,10/*mean (0.14783), correlation (0.206356)*/,
    -7,3, -5,-3/*mean (0.182141), correlation (0.198942)*/,
    -4,2, -3,7/*mean (0.188237), correlation (0.21384)*/,
    -10,-12, -6,11/*mean (0.14865), correlation (0.23571)*/,
    5,-12, 6,-7/*mean (0.222312), correlation (0.23324)*/,
    5,-6, 7,-1/*mean (0.229082), correlation (0.23389)*/,
    1,9, 4,-5/*mean (0.241577), correlation (0.215286)*/,
    9,11, 10,-10/*mean (0.00338507), correlation (0.251373)*/,
    4,7, 4,12/*mean (0.131005), correlation (0.257622)*/,
    2,-7, 4,4/*mean (0.152755), correlation (0.255205)*/,
    -4,-12, -2,7/*mean (0.182771), correlation (0.244867)*/,
    -8,-5, -7,-10/*mean (0.186898), correlation (0.23901)*/,
    4,11, 7,10/*mean (0.226226), correlation (0.258255)*/,
    0,-8, 1,-13/*mean (0.0897886), correlation (0.274827)*/,
    -13,-2, -8,2/*mean (0.148774), correlation (0.28065)*/,
    -3,-2, -2,3/*mean (0.153048), correlation (0.283063)*/,
    -6,9, -4,-9/*mean (0.169523), correlation (0.278248)*/,
    8,12, 10,7/*mean (0.225337), correlation (0.282851)*/,
    0,9, 1,3/*mean (0.226687), correlation (0.278734)*/,
    7,-5, 11,-10/*mean (0.00693882), correlation (0.305161)*/,
    -13,-6, -11,0/*mean (0.0227283), correlation (0.300181)*/,
    10,7, 12,1/*mean (0.125517), correlation (0.31089)*/,
    -6,-3, -6,12/*mean (0.131748), correlation (0.312779)*/,
    10,-9, 12,-4/*mean (0.144827), correlation (0.292797)*/,
    -10,6, -8,-12/*mean (0.149202), correlation (0.308918)*/,
    -13,0, -8,-4/*mean (0.160909), correlation (0.310013)*/,
    3,3, 7,8/*mean (0.177755), correlation (0.309394)*/,
    5,7, 10,-7/*mean (0.212337), correlation (0.310315)*/,
    -1,7, 1,-12/*mean (0.214429), correlation (0.311933)*/,
    3,-10, 5,6/*mean (0.235807), correlation (0.313104)*/,
    2,-4, 3,-10/*mean (0.00494827), correlation (0.344948)*/,
    -13,0, -13,5/*mean (0.0549145), correlation (0.344675)*/,
    -13,-7, -4,12/*mean (0.103385), correlation (0.342715)*/,
    -13,3, -11,8/*mean (0.134222), correlation (0.322922)*/,
    -7,12, -4,7/*mean (0.153284), correlation (0.337061)*/,
    6,-10, 12,8/*mean (0.154881), correlation (0.329257)*/,
    -9,-1, -7,-6/*mean (0.200967), correlation (0.33312)*/,
    -2,-5, 0,12/*mean (0.201518), correlation (0.340635)*/,
    -12,5, -7,5/*mean (0.207805), correlation (0.335631)*/,
    3,-10, 8,-13/*mean (0.224438), correlation (0.34504)*/,
    -7,-7, -4,5/*mean (0.239361), correlation (0.338053)*/,
    -3,-2, -1,-7/*mean (0.240744), correlation (0.344322)*/,
    2,9, 5,-11/*mean (0.242949), correlation (0.34145)*/,
    -5,-13, -5,-13/*mean (0.244028), correlation (0.336861)*/,
    -1,6, 7,-2/*mean (0.247571), correlation (0.343684)*/,
    5,-3, 5,2/*mean (0.000697256), correlation (0.357265)*/,
    -4,-13, -4,12/*mean (0.00213675), correlation (0.373827)*/,
    -9,-6, -9,6/*mean (0.0126856), correlation (0.373938)*/,
    -12,-10, -8,-4/*mean (0.0152497), correlation (0.364237)*/,
    10,2, 12,-3/*mean (0.0299933), correlation (0.345292)*/,
    7,12, 4,12/*mean (0.0307242), correlation (0.366299)*/,
    -7,-13, -6,5/*mean (0.0534975), correlation (0.368357)*/,
    -4,9, -3,4/*mean (0.099865), correlation (0.372276)*/,
    7,-1, 12,2/*mean (0.117083), correlation (0.364529)*/,
    -7,6, -5,1/*mean (0.126125), correlation (0.369606)*/,
    -3,11, -12,5/*mean (0.130364), correlation (0.358502)*/,
    -3,7, -2,-6/*mean (0.131691), correlation (0.375531)*/,
    7,-8, 12,-7/*mean (0.160166), correlation (0.379508)*/,
    -8,-7, -7, 11/*mean (0.167848), correlation (0.353343)*/,
    1,-3, 5,10/*mean (0.183378), correlation (0.371916)*/,
    2,-6, 3,0/*mean (0.228711), correlation (0.371761)*/,
    -4,3, -2,-13/*mean (0.247211), correlation (0.364063)*/,
    -1,-13, 1,9/*mean (0.249325), correlation (0.378139)*/,
    7,1, 8,-6/*mean (0.000652272), correlation (0.411682)*/,
    3,-5, 3,12/*mean (0.00248538), correlation (0.392988)*/,
    9,1, 12,6/*mean (0.0206815), correlation (0.386106)*/,
    -1,-9, -1,3/*mean (0.0364485), correlation (0.410752)*/,
    -6,-8, -10,5/*mean (0.0376068), correlation (0.398374)*/,
    7,7, 10,12/*mean (0.0424202), correlation (0.405663)*/,
    12,-5, 12,9/*mean (0.0942645), correlation (0.410422)*/,
    6,3, 7,11/*mean (0.1074), correlation (0.413224)*/,
    5,-13, 6,10/*mean (0.109256), correlation (0.408646)*/,
    2,-12, 2,3/*mean (0.131691), correlation (0.416076)*/,
    3,8, 4,-6/*mean (0.165081), correlation (0.417569)*/,
    2,6, 1,-13/*mean (0.171874), correlation (0.408471)*/,
    9,-12, 10,3/*mean (0.175146), correlation (0.41296)*/,
    -8,4, -7,9/*mean (0.183682), correlation (0.402956)*/,
    -11,6, -4,-6/*mean (0.184672), correlation (0.416125)*/,
    1,12, 2,-8/*mean (0.191487), correlation (0.386696)*/,
    6,-9, 7,-4/*mean (0.192668), correlation (0.394771)*/,
    2,3, 3,-2/*mean (0.200157), correlation (0.408303)*/,
    6,3, 11,0/*mean (0.204588), correlation (0.411762)*/,
    3,-3, 8,-8/*mean (0.205904), correlation (0.416294)*/,
    7,8, 9,3/*mean (0.213237), correlation (0.409306)*/,
    -11,-5, -6,-4/*mean (0.243444), correlation (0.395069)*/,
    -10,11, -5,10/*mean (0.247672), correlation (0.413392)*/,
    -5,-8, -3,12/*mean (0.24774), correlation (0.411416)*/,
    -10,5, -9,0/*mean (0.00213675), correlation (0.454003)*/,
    8,-1, 12,-6/*mean (0.0293635), correlation (0.455368)*/,
    4,-6, 6,-11/*mean (0.0404971), correlation (0.457393)*/,
    -10,12, -8,7/*mean (0.0481107), correlation (0.448364)*/,
    4,-2, 6,7/*mean (0.050641), correlation (0.455019)*/,
    -2,8, -2,12/*mean (0.0525978), correlation (0.44338)*/,
    -5,-8, -5,2/*mean (0.0629667), correlation (0.457096)*/,
    7,-6, 10,12/*mean (0.0653846), correlation (0.445623)*/,
    -9,-13, -8,-8/*mean (0.0858749), correlation (0.449789)*/,
    -5,-13, -5,-2/*mean (0.122402), correlation (0.450201)*/,
    8,-8, 9,-13/*mean (0.125416), correlation (0.453224)*/,
    -9,-11, -9,0/*mean (0.130128), correlation (0.458724)*/,
    1,-8, -6,-4/*mean (0.132467), correlation (0.440133)*/,
    7,-4, 9,1/*mean (0.132692), correlation (0.454)*/,
    -2,10, -1,-4/*mean (0.135695), correlation (0.455739)*/,
    9,-6, 5,11/*mean (0.142904), correlation (0.446114)*/,
    -12,-9, -6,4/*mean (0.146165), correlation (0.451473)*/,
    3,7, 7,12/*mean (0.147627), correlation (0.456643)*/,
    5,5, 10,8/*mean (0.152901), correlation (0.455036)*/,
    0,-4, 2,8/*mean (0.167083), correlation (0.459315)*/,
    -9,12, -5,-13/*mean (0.173234), correlation (0.454706)*/,
    0,7, 2,12/*mean (0.18312), correlation (0.433855)*/,
    -12,5, 1,7/*mean (0.185504), correlation (0.443838)*/,
    5,11, 7,-9/*mean (0.185706), correlation (0.451123)*/,
    3,5, 6,-8/*mean (0.188968), correlation (0.455808)*/,
    -13,-4, -8,9/*mean (0.191667), correlation (0.459128)*/,
    -5,9, -3,-3/*mean (0.193196), correlation (0.458364)*/,
    -4,-7, -3,-12/*mean (0.196536), correlation (0.455782)*/,
    6,5, 8,0/*mean (0.1972), correlation (0.450481)*/,
    -7,6, -6,12/*mean (0.199438), correlation (0.458156)*/,
    -13,6, -5,-2/*mean (0.211224), correlation (0.449548)*/,
    1,-10, 3,10/*mean (0.211718), correlation (0.440606)*/,
    4,1, 8,-4/*mean (0.213034), correlation (0.443177)*/,
    -4,8, 2,-13/*mean (0.234334), correlation (0.455304)*/,
    2,-12, 6,13/*mean (0.235684), correlation (0.443436)*/,
    -2,-13, 0,-6/*mean (0.237674), correlation (0.452525)*/,
    4,1, 9,3/*mean (0.23962), correlation (0.444824)*/,
    -6,-10, -3,-5/*mean (0.248459), correlation (0.439621)*/,
    -3,-11, -9,1/*mean (0.249505), correlation (0.456666)*/,
    7,5, 3,-11/*mean (0.00119208), correlation (0.495466)*/,
    4,-2, 5,-7/*mean (0.00372245), correlation (0.484214)*/,
    -13,9, -9,-5/*mean (0.00741116), correlation (0.499854)*/,
    7,1, 8,6/*mean (0.0208952), correlation (0.499773)*/,
    7,-8, 7,6/*mean (0.0220085), correlation (0.501609)*/,
    -7,-4, -7,1/*mean (0.0233806), correlation (0.496568)*/,
    -8,11, -7,-8/*mean (0.0236505), correlation (0.489719)*/,
    -13,6, -12,-8/*mean (0.0268781), correlation (0.503487)*/,
    2,4, 3,9/*mean (0.0323324), correlation (0.501938)*/,
    10,-5, 12,3/*mean (0.0399235), correlation (0.494029)*/,
    -6,-5, -6,7/*mean (0.0420153), correlation (0.486579)*/,
    8,-3, 9,-8/*mean (0.0548021), correlation (0.484237)*/,
    2,-12, 2,8/*mean (0.0616622), correlation (0.496642)*/,
    -11,-2, -10,3/*mean (0.0627755), correlation (0.498563)*/,
    -9,10, -7,-9/*mean (0.0829622), correlation (0.495491)*/,
    -11,0, -10,-5/*mean (0.0843342), correlation (0.487146)*/,
    5,-3, 11,8/*mean (0.0929937), correlation (0.502315)*/,
    -2,-13, -1,12/*mean (0.113327), correlation (0.48941)*/,
    -1,-8, 0,9/*mean (0.132119), correlation (0.467268)*/,
    -3,-11, -12,-5/*mean (0.136269), correlation (0.498771)*/,
    -10,-2, -10,11/*mean (0.142173), correlation (0.498714)*/,
    -3,9, -2,-13/*mean (0.144141), correlation (0.491973)*/,
    2,-3, 3,2/*mean (0.14892), correlation (0.500782)*/,
    -9,-13, -4,0/*mean (0.150371), correlation (0.498211)*/,
    -4,6, -3,-10/*mean (0.152159), correlation (0.495547)*/,
    -4,12, -2,-7/*mean (0.156152), correlation (0.496925)*/,
    -6,-11, -4,9/*mean (0.15749), correlation (0.499222)*/,
    6,-3, 6,11/*mean (0.159211), correlation (0.503821)*/,
    -13,1, -5,5/*mean (0.162427), correlation (0.501907)*/,
    11,11, 12,6/*mean (0.16652), correlation (0.497632)*/,
    7,-5, 12,-2/*mean (0.169141), correlation (0.484474)*/,
    -1,12, 0,7/*mean (0.169456), correlation (0.495339)*/,
    -4,-8, -3,-2/*mean (0.171457), correlation (0.487251)*/,
    -7,1, -6,7/*mean (0.175), correlation (0.500024)*/,
    -1,-12, -8,-13/*mean (0.175866), correlation (0.497523)*/,
    -7,-2, -6,-8/*mean (0.178273), correlation (0.501854)*/,
    -8,5, -6,-9/*mean (0.181107), correlation (0.494888)*/,
    -5,-1, -4,5/*mean (0.190227), correlation (0.482557)*/,
    -13,7, -8,10/*mean (0.196739), correlation (0.496503)*/,
    1,5, 5,-13/*mean (0.19973), correlation (0.499759)*/,
    1,9, 9,-4/*mean (0.204465), correlation (0.49873)*/,
    9,12, 10,-1/*mean (0.209334), correlation (0.49063)*/,
    5,-8, 10,-9/*mean (0.211134), correlation (0.503011)*/,
    -1,11, 1,-13/*mean (0.212), correlation (0.499414)*/,
    -9,-3, -6,2/*mean (0.212168), correlation (0.480739)*/,
    -1,-10, 1,12/*mean (0.212731), correlation (0.502523)*/,
    -13,1, -8,-10/*mean (0.21327), correlation (0.489786)*/,
    8,-11, 10,-6/*mean (0.214159), correlation (0.488246)*/,
    2,-13, 3,-6/*mean (0.216993), correlation (0.50287)*/,
    7,-13, 12,-9/*mean (0.223639), correlation (0.470502)*/,
    -10,-10, -5,-7/*mean (0.224089), correlation (0.500852)*/,
    -10,-8, -8,-13/*mean (0.228666), correlation (0.502629)*/,
    4,-6, 8,5/*mean (0.22906), correlation (0.498305)*/,
    3,12, 8,-13/*mean (0.233378), correlation (0.503825)*/,
    -4,2, -3,-3/*mean (0.234323), correlation (0.476692)*/,
    5,-13, 10,-12/*mean (0.236392), correlation (0.475462)*/,
    4,-13, 5,-1/*mean (0.236842), correlation (0.504132)*/,
    -9,9, -4,3/*mean (0.236977), correlation (0.497739)*/,
    0,3, 3,-9/*mean (0.24314), correlation (0.499398)*/,
    -12,1, -6,1/*mean (0.243297), correlation (0.489447)*/,
    3,2, 4,-8/*mean (0.00155196), correlation (0.553496)*/,
    -10,-10, -10,9/*mean (0.00239541), correlation (0.54297)*/,
    8,-13, 8,11/*mean (0.0034413), correlation (0.544361)*/,
    -8,-12, -6,-5/*mean (0.003565), correlation (0.551225)*/,
    2,4, 3,7/*mean (0.00835583), correlation (0.55285)*/,
    10,6, 11,-8/*mean (0.00885065), correlation (0.540913)*/,
    6,8, 8,-12/*mean (0.0101552), correlation (0.551085)*/,
    -7,10, -6,5/*mean (0.0102227), correlation (0.533635)*/,
    -3,-9, -3,9/*mean (0.0110211), correlation (0.543121)*/,
    -1,-13, -1,5/*mean (0.0113473), correlation (0.550173)*/,
    -3,-7, -3,4/*mean (0.0140913), correlation (0.554774)*/,
    -8,-2, -8,3/*mean (0.017049), correlation (0.55461)*/,
    4,2, 8,10/*mean (0.01778), correlation (0.546921)*/,
    2,-5, 3,11/*mean (0.0224022), correlation (0.549667)*/,
    6,-9, 5,6/*mean (0.029161), correlation (0.546295)*/,
    3,-1, 7,12/*mean (0.0303081), correlation (0.548599)*/,
    11,-1, 12,4/*mean (0.0355151), correlation (0.523943)*/,
    -3,0, -3,6/*mean (0.0417904), correlation (0.543395)*/,
    4,-11, 4,12/*mean (0.0487292), correlation (0.542818)*/,
    2,-4, 2,11/*mean (0.0575124), correlation (0.554888)*/,
    -10,-6, -8,1/*mean (0.0594242), correlation (0.544026)*/,
    -13,7, -11,1/*mean (0.0597391), correlation (0.550524)*/,
    -9,8, -1,-3/*mean (0.0608974), correlation (0.55383)*/,
    6,0, 1,-13/*mean (0.065126), correlation (0.552006)*/,
    6,-1, 1,4/*mean (0.074224), correlation (0.546372)*/,
    -13,3, -9,-2/*mean (0.0808592), correlation (0.554875)*/,
    -9,8, -6,-3/*mean (0.0883378), correlation (0.551178)*/,
    -13,-6, -8,-2/*mean (0.0901035), correlation (0.548446)*/,
    5,-9, 8,10/*mean (0.0949843), correlation (0.554694)*/,
    2,7, 3,-9/*mean (0.0994152), correlation (0.550979)*/,
    -1,-6, -10,-1/*mean (0.10045), correlation (0.552714)*/,
    9,5, 11,-2/*mean (0.100686), correlation (0.552594)*/,
    11,-3, 12,-8/*mean (0.101091), correlation (0.532394)*/,
    3,0, 3,5/*mean (0.101147), correlation (0.525576)*/,
    -1,4, 0,10/*mean (0.105263), correlation (0.531498)*/,
    3,-6, 4,5/*mean (0.110785), correlation (0.540491)*/,
    -13,0, -10,5/*mean (0.112798), correlation (0.536582)*/,
    5,8, 1,11/*mean (0.114181), correlation (0.555793)*/,
    8,9, 9,-6/*mean (0.117431), correlation (0.553763)*/,
    7,-4, 8,-12/*mean (0.118522), correlation (0.553452)*/,
    -10,4, -10,9/*mean (0.12094), correlation (0.554785)*/,
    7,3, 12,4/*mean (0.122582), correlation (0.555825)*/,
    9,-7, 10,-2/*mean (0.124978), correlation (0.549846)*/,
    7,0, 12,-2/*mean (0.127002), correlation (0.537452)*/,
    -1,-6, 0,-11/*mean (0.127148), correlation (0.547401)*/
};
static int bit_pattern_list[512] = {
9,10,4,14,14,8,14,14,13,12,7,6,10,4,11,14,
13,15,11,14,15,12,13,15,10,13,6,3,13,12,9,9,
13,12,7,12,14,9,14,14,8,5,6,10,8,13,11,6,
11,10,9,13,8,6,4,8,16,13,13,9,8,7,9,6,
14,14,8,13,7,6,13,7,9,12,12,12,8,13,13,8,
4,4,11,10,14,12,9,3,9,15,14,11,12,12,7,13,
13,13,12,14,13,9,4,11,9,12,7,12,10,8,4,10,
13,14,15,13,13,14,14,8,12,14,9,9,5,12,13,9,
10,15,10,6,4,7,9,12,14,14,6,7,6,5,14,13,
11,11,16,9,10,12,14,13,15,8,10,5,7,12,9,5,
11,13,8,6,11,14,11,13,3,11,6,3,5,13,13,9,
7,10,6,12,9,13,9,3,10,11,10,16,13,15,7,13,
14,12,12,4,9,7,6,13,15,10,9,11,13,7,12,8,
11,8,4,4,7,11,4,11,11,9,12,7,15,11,9,12,
11,9,8,13,7,13,16,11,4,9,8,12,9,5,9,16,
16,11,14,5,11,16,14,9,8,7,8,9,10,4,11,12,
15,7,8,14,7,13,4,8,15,14,7,12,13,7,12,11,
6,10,14,12,10,4,8,12,8,8,9,13,14,5,10,10,
4,9,9,13,12,14,13,6,4,9,12,6,11,9,9,11,
4,9,16,10,7,10,11,9,8,7,14,11,14,14,4,9,
11,12,8,9,9,12,12,8,11,10,13,11,11,11,6,14,
13,12,8,9,11,13,10,15,9,13,4,4,16,4,7,10,
13,7,13,10,7,13,13,7,16,13,9,12,12,7,9,4,
7,9,12,15,7,10,9,11,5,6,15,13,5,14,9,10,
15,10,9,13,11,13,9,6,10,12,13,13,14,12,13,7,
15,15,14,9,13,15,7,9,12,15,4,4,14,16,14,5,
13,5,3,9,12,6,4,9,14,13,15,14,14,8,4,8,
12,14,10,14,12,8,9,9,13,5,8,5,8,9,4,13,
5,11,11,8,3,14,11,13,3,7,12,13,4,11,12,8,
15,11,12,3,6,13,6,4,13,9,12,7,14,8,10,13,
7,9,6,10,10,11,11,14,3,6,4,10,7,6,13,11,
9,11,12,11,8,14,11,13,8,13,11,10,7,12,6,11
};

static const int INPUT_W = 480;
static const int INPUT_H = 320;
static const int SLICE_TIMES = 30;
static const int MAX_ST = SLICE_TIMES*SLICE_TIMES;
static const int BLOCK_SIZE = 7;
static const float HARRIS_K = 0.04;
static const int PATCH_SIZE = 31;
static const int HALF_PATCH_SIZE = 10;
static const int MATCHING_FILTER = 24;
static const int STP = 15;
static const int DIS = 1;
static const int MAX_CONNECT = 80;


double START[5] = { 0 }, TOTAL[5] = { 0 };
unsigned char input[5][INPUT_H][INPUT_W] = { 0 };

struct Keypoint
{
    float distance = 0;
    float index = 0;
    float angle = -1;
    float response = 0;
    int axis[2] = { 0 };
    unsigned char descriptors[16] = { 0 };
    bool po = false;
};

struct SaveKnP
{
    int b_X = 0;
    int b_Y = 0;
    int n_X = 0;
    int n_Y = 0;
};

struct Pattern
{
    int x = { 0 };
    int y = { 0 };
};

Mat image;
Mat Output = Mat(INPUT_H * 2, INPUT_W, CV_32FC1, 0.0f);
String OutSign = "Display";

//void convolution(unsigned char[INPUT_H][INPUT_W], unsigned char*, int, int, float[]);
void cover_to_gray(Mat*, uchar*, int, int);
//uint16_t briefSelect(uchar*, int H, int W, int thita, int[16][2], int[16][2], int);
//void JacobiSVD1(std::vector<std::vector<float>>& At, std::vector<std::vector<float>>& _W, std::vector<std::vector<float>>& Vt);
//int svd1(const std::vector<std::vector<float>>& matSrc, std::vector<std::vector<float>>& matD, std::vector<std::vector<float>>& matU, std::vector<std::vector<float>>& matVt);
void connect_keypoint(Keypoint, int*, float);
int cornerScore(unsigned char*, int[], int);
void gaussianBlur1(unsigned char*, int, int);
void gaussianBlur2(unsigned char*, int, int);
void Keypoint_Sort(Keypoint*);
float fastatan2(float, float);

void print_matrix(const vector<vector<float>>& vec);
void print_array(const float* vec, int m, int n);
void print_mat(const cv::Mat vec);
void jacobiSVD(float* At, float* _W, float* Vt, const int m, const int n);
int svd(float* matSrc, float* matD, float* matU, float* matVt, const int tm, const int tn);
void inv(float* In_mat, float* Out_mat, const int n);
void matMult(float* In_mat1, float* In_mat2, float* Out_mat, const int m, const int n, const int o);
bool reconstructH1(float* H21, float* K, SaveKnP* saveKnP);
bool reconstructH(cv::Mat& H21, cv::Mat& K, SaveKnP* saveKnP);
bool reconstructF1(float* F21, float* K, SaveKnP* saveKnP);
bool reconstructF(cv::Mat& F21, cv::Mat& K, SaveKnP* saveKnP);
void checkRT1(SaveKnP* saveKnP, float* K, float* R, float* t);
void checkRT(SaveKnP* saveKnP, const cv::Mat& K, const cv::Mat& R, const cv::Mat& t);
int oopoo = 0;
int tg_oopoo = 0;

void initKeyPoint();
void findKeyPoint(SaveKnP* saveKnP);

unsigned char buf[3][INPUT_W] = { 0 };
float ang_buf[3][INPUT_W] = { 0 };
int cpbuf[3][INPUT_W+1] = { 0 };
int ofs[BLOCK_SIZE * BLOCK_SIZE] = { 0 };
unsigned char threshold_tab[512];
float scale_sq_sq = 0;
int umax[15 + 2] = { 0 };
int pixel[25];
int connectTimes = 0;
Keypoint keypoint[MAX_ST];
float io = 0;
float xp = 0;
float yp = 0;
float zp = 0;
float xpt = 0;
float ypt = 0;
float zpt = 0;
float xt = 0;
float yt = 0;
float zt = 0;
float xtt = 0;
float ytt = 0;
float ztt = 0;
int saveKnPi[MAX_CONNECT][4] = { 0 };
float saveKnPD[MAX_CONNECT] = { 0 };
int saveR[MAX_CONNECT] = { 0 };  //紀錄配對結果
int saveD[MAX_CONNECT] = { 0 };  //紀錄每個特徵點相距長度
SaveKnP saveKnP[MAX_CONNECT] = { 0 };
float input2[INPUT_H][INPUT_W] = { 0 };


int main()
{ 
    cout << sizeof(keypoint[0]) << endl;
    //與 fast 算法相關
    for (int i = -255; i <= 255; i++)
        threshold_tab[i + 255] = (unsigned char)(i < -10 ? 1 : i > 10 ? 2 : 0);

    //與 harris 算法相關
    for (int i = 0; i < BLOCK_SIZE; i++)
        for (int j = 0; j < BLOCK_SIZE; j++)
            ofs[i * BLOCK_SIZE + j] = i * INPUT_W + j;
    float scale = 1.f / (4 * BLOCK_SIZE * 255.f);
    scale_sq_sq = scale * scale * scale * scale;

    //與 IC_Angel 算法相關，計算出 umax 為主軸，尋找一個圓的?層y軸有多少框框在此圓中
    //ex:半徑為15的圓，當y=1時，有15個框框在此圓以內，當y=14時，有3個框框在此圓以內
    int v, v0, vmax = floor(HALF_PATCH_SIZE * std::sqrt(2.f) / 2 + 1);
    int vmin = ceil(HALF_PATCH_SIZE * std::sqrt(2.f) / 2);
    for (v = 0; v <= vmax; ++v) {
        umax[v] = round(std::sqrt((double)HALF_PATCH_SIZE * HALF_PATCH_SIZE - v * v));
    }
    for (int v = HALF_PATCH_SIZE, v0 = 0; v >= vmin; --v)
    {
        while (umax[v0] == umax[v0 + 1])
            ++v0;
        umax[v] = v0;
        ++v0;
    }

    //在以3為半徑上共有16個點，但是為了方便25後面8個是最前面的8個
    for (int i = 0; i < 16; i++)
        pixel[i] = k_size_3[i][0] + k_size_3[i][1] * INPUT_W;
    for (int i = 16; i < 25; i++)
        pixel[i] = pixel[i - 16];

    initKeyPoint();
    oopoo += 1;
    tg_oopoo = oopoo;
    for (int i = 1; i < 159; i++) {
        xp = 0; yp = 0; zp = 0;
        oopoo = i;
        for (int i = 0; i < MAX_CONNECT; i++) {
            saveKnP[i].b_X = 0;
            saveKnP[i].b_Y = 0;
            saveKnP[i].n_X = 0;
            saveKnP[i].n_Y = 0;
        }


        for (int o = 0; o < MAX_CONNECT; o++) {
            for (int po = 0; po < 4; po++) {
                saveKnPi[o][po] = 0;
            }
        }
        findKeyPoint(saveKnP);

        const float th = 3.841;
        const float thScore = 5.991;

        float H_A[16][9] = { 0 };
        float H_w[9][1] = { 0 };
        float H_u[16][16] = { 0 };
        float H_vt[9][9] = { 0 };
        float H[3][3] = { 0 };

        float F_A[8][9] = { 0 };
        float F_w[9][1] = { 0 };
        float F_u[8][8] = { 0 };
        float F_vt[9][9] = { 0 };
        float FS[3][3] = { 0 };
        float F[3][3] = { 0 };

        float score, BL = -1000000;

        if (connectTimes >= 16) {
            for (int j = 0; j < 8; j++)
            {
                float HTo[3][3] = { 0 };

                for (int k = 0; k < 8; k++) {
                    int u1 = saveKnP[k + j].b_X;
                    int v1 = saveKnP[k + j].b_Y;
                    int u2 = saveKnP[k + j].n_X;
                    int v2 = saveKnP[k + j].n_Y;

                    H_A[k * 2][0] = u1;
                    H_A[k * 2][1] = v1;
                    H_A[k * 2][2] = 1;
                    H_A[k * 2][3] = 0;
                    H_A[k * 2][4] = 0;
                    H_A[k * 2][5] = 0;
                    H_A[k * 2][6] = -u1 * u2;
                    H_A[k * 2][7] = -v1 * u2;
                    H_A[k * 2][8] = -u2;

                    H_A[k * 2 + 1][0] = 0;
                    H_A[k * 2 + 1][1] = 0;
                    H_A[k * 2 + 1][2] = 0;
                    H_A[k * 2 + 1][3] = -u1;
                    H_A[k * 2 + 1][4] = -v1;
                    H_A[k * 2 + 1][5] = -1;
                    H_A[k * 2 + 1][6] = u1 * v2;
                    H_A[k * 2 + 1][7] = v1 * v2;
                    H_A[k * 2 + 1][8] = v2;
                }
                svd(*H_A, *H_w, *H_u, *H_vt, 16, 9);
                for (int k = 0; k < 3; k++)
                    for (int l = 0; l < 3; l++)
                        HTo[k][l] = H_vt[8][k * 3 + l] / H_vt[8][8];
                score = 0;
                float invHTo[3][3] = {{ 1,0,0 }, { 0,1,0 }, { 0,0,1 }};
                inv(*HTo, *invHTo,3);
                for (int k = 0; k < 8; k++) {
                    int u1 = saveKnP[k + j].b_X;
                    int v1 = saveKnP[k + j].b_Y;
                    int u2 = saveKnP[k + j].n_X;
                    int v2 = saveKnP[k + j].n_Y;
                    float dis = (u1 * HTo[2][0] + v1 * HTo[2][1] + 1 * HTo[2][2]);
                    score += -(pow(u2 - (u1 * HTo[0][0] + v1 * HTo[0][1] + 1 * HTo[0][2]) / dis, 2) +
                        pow(v2 - (u1 * HTo[1][0] + v1 * HTo[1][1] + 1 * HTo[1][2]) / dis, 2));

                    dis = (u2 * invHTo[2][0] + v2 * invHTo[2][1] + 1 * invHTo[2][2]);

                    score += -(pow(u1 - (u2 * invHTo[0][0] + v2 * invHTo[0][1] + 1 * invHTo[0][2]) / dis, 2) +
                        pow(v1 - (u2 * invHTo[1][0] + v2 * invHTo[1][1] + 1 * invHTo[1][2]) / dis, 2));
                }
                
                
                if (BL < score) {
                    for (int i = 0; i < 3; i++) {
                        for (int k = 0; k < 3; k++) {
                            H[i][k] = HTo[i][k];
                        }
                    }
                    BL = score;
                }

                //if (loss < 2) break;
                //cout << loss << " , ";
            }
            //cout << BL;
            //cout << endl;
            //cout << "H_Loss:" << "" << loss << endl;

            //--------------------------------------------------------------------------------------

            BL = -10000000;
            TOTAL[3] = getTickCount();
            for (int j = 0; j < 40; j++) {
                float FTo[3][3] = { 0 };

                srand(j + i * 1000);
                int no, randN[8] = { 0 };
                randN[0] = rand() % (connectTimes + 1);
                for (int i = 1; i < 8; i++) {
                    while (1) {
                        bool oq = true;
                        no = rand() % (connectTimes + 1);
                        for (int k = 0; k < i; k++)
                            if (randN[k] == no)
                                oq = false;
                        if (oq == true)
                            break;
                    }
                    randN[i] = no;
                }

                for (int k = 0; k < 8; k++) {
                    int u1 = saveKnP[randN[k]].b_X;
                    int v1 = saveKnP[randN[k]].b_Y;
                    int u2 = saveKnP[randN[k]].n_X;
                    int v2 = saveKnP[randN[k]].n_Y;

                    F_A[k][0] = u2 * u1;
                    F_A[k][1] = u2 * v1;
                    F_A[k][2] = u2;
                    F_A[k][3] = v2 * u1;
                    F_A[k][4] = v2 * v1;
                    F_A[k][5] = v2;
                    F_A[k][6] = u1;
                    F_A[k][7] = v1;
                    F_A[k][8] = 1;
                }
                svd(*F_A, *F_w, *F_u, *F_vt, 8, 9);
                for (int k = 0; k < 3; k++)
                    for (int l = 0; l < 3; l++)
                        FS[k][l] = F_vt[8][k * 3 + l];
                svd(*FS, *F_w, *F_u, *F_vt, 3, 3);
                float wI[3][3] = { {F_w[0][0],0,0},{0,F_w[1][0],0},{0,0,0} };
                matMult(*F_u, *wI, *FS, 3, 3, 3);
                matMult(*FS, *F_vt, *FTo, 3, 3, 3);
                for (int k = 0; k < 3; k++)
                    for (int l = 0; l < 3; l++)
                        FTo[k][l] = FTo[k][l] / FTo[2][2];

                score = 0;
                for (int k = 0; k < 8; k++) {
                    int u1 = saveKnP[k + j].b_X;
                    int v1 = saveKnP[k + j].b_Y;
                    int u2 = saveKnP[k + j].n_X;
                    int v2 = saveKnP[k + j].n_Y;

                    float a = FTo[0][0] * u1 + FTo[0][1] * v1 + FTo[0][2];
                    float b = FTo[1][0] * u1 + FTo[1][1] * v1 + FTo[1][2];
                    float c = FTo[2][0] * u1 + FTo[2][1] * v1 + FTo[2][2];
                    float num = a * u2 + b * v2 + c;
                    float squareDist = num * num / (a * a + b * b);
                    score += - squareDist;

                    a = FTo[0][0] * u2 + FTo[1][0] * v2 + FTo[2][0];
                    b = FTo[0][1] * u2 + FTo[1][1] * v2 + FTo[2][1];
                    c = FTo[0][2] * u2 + FTo[1][2] * v2 + FTo[2][2];
                    num = a * u1 + b * v1 + c;
                    squareDist = num * num / (a * a + b * b);
                    score += - squareDist;
                }

                if (BL < score) {
                    for (int i = 0; i < 3; i++) {
                        for (int k = 0; k < 3; k++) {
                            F[i][k] = FTo[i][k];
                        }
                    }
                    BL = score;
                }
            }
            //cout << " , " << BL << endl;
            //cout << (getTickCount() - TOTAL[3]) / getTickFrequency() << endl;
            //--------------------------------------------------------------------------------------

            Mat Ha = (Mat_<float>(3, 3) <<
                H[0][0], H[0][1], H[0][2],
                H[1][0], H[1][1], H[1][2],
                H[2][0], H[2][1], H[2][2]);

            Mat K2 = (Mat_<float>(3, 3) <<
                275.5, 0, INPUT_W/2,
                0, 275.5, INPUT_H/2,
                0, 0, 1);

            Mat Fa = (Mat_<float>(3, 3) <<
                F[0][0], F[0][1], F[0][2],
                F[1][0], F[1][1], F[1][2],
                F[2][0], F[2][1], F[2][2]);

            float K[3][3] = { {275.5,0,INPUT_W/2},{0,275.5,INPUT_H/2},{0,0,1} };

            reconstructF(Fa, K2, saveKnP);
            //reconstructH(Ha, K2, saveKnP);
            reconstructH1(*H, *K, saveKnP);
            reconstructF1(*F, *K, saveKnP);

            //--------------------------------------------------------------------------------------
        }


        //int(float(0 - (INPUT_H / 2)) * 0.6 + (INPUT_H / 2))  int(float(0 - (INPUT_W / 2)) * 0.6  + (INPUT_W / 2))
        float dis = 0;
        for (int i = 0; i < INPUT_H; i++) {
            for (int j = 0; j < INPUT_W; j++) {
                dis = (H[2][0] * j) + (H[2][1] * i) + (H[2][2] * 1);
                int y = int(((H[0][0] * j) + (H[0][1] * i) + (H[0][2] * 1)) / dis);
                int x = int(((H[1][0] * j) + (H[1][1] * i) + (H[1][2] * 1)) / dis);
                if (x >= 0 && x < INPUT_H && y >= 0 && y < INPUT_W) {
                    input2[i][j] = input[1][x][y];
                }
                else {
                    input2[i][j] = 0;
                }
            }
        }

        int lrud[4][2] = { {0,0} ,{INPUT_W,0} ,{0,INPUT_H} ,{INPUT_W,INPUT_H} };
        int outlrud[4][2] = { {0,0} ,{INPUT_W,0} ,{0,INPUT_H} ,{INPUT_W,INPUT_H} };

        for (int k = 0; k < 4; k++) {
            int i = lrud[k][0];
            int j = lrud[k][1];
            dis = (H[2][0] * i + (H[2][1] * j) + (H[2][2] * 1));
            outlrud[k][0] = int(((H[0][0] * i) + (H[0][1] * j) + (H[0][2] * 1)) / dis);
            outlrud[k][1] = int(((H[1][0] * i) + (H[1][1] * j) + (H[1][2] * 1)) / dis);
        }
        float stpX = (outlrud[0][0] + outlrud[1][0] + outlrud[2][0] + outlrud[3][0]) - 2 * INPUT_W;
        float stpY = (outlrud[0][1] + outlrud[1][1] + outlrud[2][1] + outlrud[3][1]) - 2 * INPUT_H;
        float stpXDis = (outlrud[1][0] - outlrud[0][0]) - (outlrud[3][0] - outlrud[2][0]);
        float stpYDis = (outlrud[2][1] - outlrud[0][1]) - (outlrud[3][1] - outlrud[1][1]);
        //cout << "x: " << xp << " , " << "y: " << yp << " , " << "z: " << zp << " , " << endl;
        //cout << xt << " , " << yt << " , " << zt << " , " << endl;
        //cout << -stpX/10 << " , " << stpY/10 << " , " << stpYDis / 10 << " , " << -stpXDis / 10 << " , " << endl;
        //cout << xt << " , " << yt << " , " << zt << " , " << endl;
        //cout << xpt << " , " << ypt << " , " << zpt << " , " << endl;
        //cout << xtt << " , " << ytt << " , " << ztt << " , " << endl;

        for (int h = 0; h < INPUT_H; ++h) {
            for (int w = 0; w < INPUT_W; ++w) {
                Output.at<float>(h, w) = input[0][h][w] / 255.0;
                //Output.at<float>(w, h + INPUT_W) = input[1][h][w];
            }
        }

        for (int h = 0; h < INPUT_H; ++h) {
            for (int w = 0; w < INPUT_W; ++w) {
                Output.at<float>(h + INPUT_H, w) = input[1][h][w] / 255.0;
                //Output.at<float>(h + INPUT_H, w) = input[1][h][w] / 255.0;
                //Output.at<float>(h + INPUT_H, w) = input[1][h][w] / 255.0;
            }
        }

        for (int p = 0; p < MAX_CONNECT; p++) {
            arrowedLine(Output,
                Point(saveKnP[p].b_X, saveKnP[p].b_Y),
                Point(saveKnP[p].n_X, saveKnP[p].n_Y + INPUT_H),
                Scalar(0), 1, 8, 0, 0.04);
        }


        if (connectTimes < 16) {
            cout << "Too Little" << endl;
            tg_oopoo = oopoo;
            initKeyPoint();
        }
        



        for (int i = 0; i < MAX_CONNECT; i++) {
            saveKnPD[i] = 0;
        }

        cv::imshow(OutSign, Output);
        //imwrite("output.jpg", Output * 255);

        cv::waitKey(0);
    }

    /*
    float angler = 110;
    if (angler <= 180) {
        angler = -angler + 180;
    }
    else {
        angler = (-angler + 180) + 360;
    }
    angler /= 360;
    
    float anglew = 200;
    if (anglew <= 180) {
        anglew = -anglew + 180;
    }
    else {
        anglew = (-anglew + 180) + 360;
    }
    anglew /= 360;
    for (int i = 0; i< 512; i++) {
        if (i % 16 == 0) {
            //cout << endl;
        }
        //int r = round(sqrt(_pattern[i].x * _pattern[i].x + _pattern[i].y * _pattern[i].y));
        int r = bit_pattern_list[i];
        //cout << r << ",";
        if (i < 1) {
            
            cout << int(k_size[bit_pattern_list[i]][i + int(angler * k_list[bit_pattern_list[i]])][0]) << "," << int(k_size[bit_pattern_list[i]][i + int(angler * k_list[bit_pattern_list[i]])][1])  << endl;
            cout << int(k_size[bit_pattern_list[i]][i + int(anglew * k_list[bit_pattern_list[i]])][0]) << "," << int(k_size[bit_pattern_list[i]][i + int(anglew * k_list[bit_pattern_list[i]])][1]) << endl;
            /*
            cout << _pattern[i].x << "," << _pattern[i].y << ",";
            cout << int(k_list[r]) << ",";
            if (_pattern[i].x > 0) {
                cout << int(k_size[r][int(k_list[r] *(-atan2(_pattern[i].y, _pattern[i].x) / (2 * 3.14159) + 0.25))][0] )
                    << "," << int(k_size[r][int(k_list[r] * (-atan2(_pattern[i].y, _pattern[i].x) / (2 * 3.14159) + 0.25))][1]) << endl;
            }
            else if (_pattern[i].x == 0) {
                if (_pattern[i].y < 0) {
                    cout << int(k_size[r][int(k_list[r] * 0.5)][0]) << "," << int(k_size[r][int(k_list[r] * 0.5)][1]) << endl;
                }
                else {
                    cout << int(k_size[r][0][0]) << "," << int(k_size[r][0][1]) << endl;
                }
            }
            else {
                cout << int(k_size[r][int(k_list[r] * (-atan2(-_pattern[i].y, -_pattern[i].x) / (2*3.14159) + 0.75))][0])
                    << "," << int(k_size[r][int(k_list[r] * (-atan2(-_pattern[i].y, -_pattern[i].x) / (2 * 3.14159) + 0.75))][1]) << endl;
            }
        } 
    }

    TOTAL = 0;
    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            input[1][i][j] = tInput[i][j];
        }
    }
    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            input[0][i][j] = hInput[i][j];
        }
    }

    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            input[1][i][j] = tInput[i][j];
        }
    }
    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            input[0][i][j] = hInput[i][j];
        }
    }

    //--------------------------------------------------------------------------------------

    std::cout << TOTAL[3] / getTickFrequency() << endl;

    float H_A[16][9] = { 0 };
    float H_w[9][1] = { 0 };
    float H_u[16][16] = { 0 };
    float H_vt[9][9] = { 0 };
    float H[3][3] = { 0 };

    float F_A[8][9] = { 0 };
    float F_w[9][1] = { 0 };
    float F_u[8][8] = { 0 };
    float F_vt[9][9] = { 0 };
    float FS[3][3] = { 0 };
    float F[3][3] = { 0 };

    if (connectTimes >= 8) {
        float loss;
        for (int j = 0; j < 8; j++)
        {
            for (int i = 0; i < 8; i++) {
                int u1 = saveKnP[i + j][0];
                int v1 = saveKnP[i + j][1];
                int u2 = saveKnP[i + j][2];
                int v2 = saveKnP[i + j][3];

                H_A[i * 2][0] = u1;
                H_A[i * 2][1] = v1;
                H_A[i * 2][2] = 1;
                H_A[i * 2][3] = 0;
                H_A[i * 2][4] = 0;
                H_A[i * 2][5] = 0;
                H_A[i * 2][6] = -u1 * u2;
                H_A[i * 2][7] = -v1 * u2;
                H_A[i * 2][8] = -u2;

                H_A[i * 2 + 1][0] = 0;
                H_A[i * 2 + 1][1] = 0;
                H_A[i * 2 + 1][2] = 0;
                H_A[i * 2 + 1][3] = -u1;
                H_A[i * 2 + 1][4] = -v1;
                H_A[i * 2 + 1][5] = -1;
                H_A[i * 2 + 1][6] = u1 * v2;
                H_A[i * 2 + 1][7] = v1 * v2;
                H_A[i * 2 + 1][8] = v2;
            }
            svd(*H_A, *H_w, *H_u, *H_vt, 16, 9);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    H[i][j] = H_vt[8][i * 3 + j] / H_vt[8][8];
            loss = 0;
            for (int i = 0; i < 8; i++) {
                int u1 = saveKnP[i + j][0];
                int v1 = saveKnP[i + j][1];
                int u2 = saveKnP[i + j][2];
                int v2 = saveKnP[i + j][3];
                float dis = (u1 * H[2][0] + v1 * H[2][1] + 1 * H[2][2]);
                loss += pow(u2 - (u1 * H[0][0] + v1 * H[0][1] + 1 * H[0][2]) / dis, 2) +
                    pow(v2 - (u1 * H[1][0] + v1 * H[1][1] + 1 * H[1][2]) / dis, 2);
            }
            if (loss < 50) break;
        }

        cout << "H_Loss:" << "" << loss << endl;

        //--------------------------------------------------------------------------------------

        const float th = 3.841;
        const float thScore = 5.991;
        for (int j = 0; j < 8; j++) {
            for (int i = 0; i < 8; i++) {
                int u1 = saveKnP[i + j][0];
                int v1 = saveKnP[i + j][1];
                int u2 = saveKnP[i + j][2];
                int v2 = saveKnP[i + j][3];

                F_A[i][0] = u2 * u1;
                F_A[i][1] = u2 * v1;
                F_A[i][2] = u2;
                F_A[i][3] = v2 * u1;
                F_A[i][4] = v2 * v1;
                F_A[i][5] = v2;
                F_A[i][6] = u1;
                F_A[i][7] = v1;
                F_A[i][8] = 1;
            }
            svd(*F_A, *F_w, *F_u, *F_vt, 8, 9);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    FS[i][j] = F_vt[8][i * 3 + j];
            svd(*FS, *F_w, *F_u, *F_vt, 3, 3);
            float wI[3][3] = { {F_w[0][0],0,0},{0,F_w[1][0],0},{0,0,0} };
            matMult(*F_u, *wI, *FS, 3, 3, 3);
            matMult(*FS, *F_vt, *F, 3, 3, 3);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    F[i][j] = F[i][j] / F[2][2];

            loss = 0;
            for (int i = 0; i < 8; i++) {
                int u1 = saveKnP[i + j][0];
                int v1 = saveKnP[i + j][1];
                int u2 = saveKnP[i + j][2];
                int v2 = saveKnP[i + j][3];

                float a = F[0][0] * u1 + F[0][1] * v1 + F[0][2];
                float b = F[1][0] * u1 + F[1][1] * v1 + F[1][2];
                float c = F[2][0] * u1 + F[2][1] * v1 + F[2][2];
                float num = a * u2 + b * v2 + c;
                float squareDist = num * num / (a * a + b * b);
                loss += squareDist;

                a = F[0][0] * u2 + F[0][1] * v2 + F[0][2];
                b = F[1][0] * u2 + F[1][1] * v2 + F[1][2];
                c = F[2][0] * u2 + F[2][1] * v2 + F[2][2];
                num = a * u1 + b * v1 + c;
                squareDist = num * num / (a * a + b * b);
                loss += squareDist;
            }
            cout << loss << endl;
            if (loss < 15000) {
                break;
            }
        }

        //--------------------------------------------------------------------------------------

        print_array(*H, 3, 3);
        print_array(*F, 3, 3);

        float K[3][3] = { {400,0,INPUT_W / 2},{0,400,INPUT_H / 2},{0,0,1} };
        reconstructH1(*H, *K);
        reconstructF1(*F, *K);

        //--------------------------------------------------------------------------------------
    }
    else {
        cout << "Too Little" << endl;
    }

    float dis = 0;
    float input2[INPUT_H][INPUT_W] = { 0 };
    for (int i = 0; i < INPUT_H; i++) {
        for (int j = 0; j < INPUT_W; j++) {
            dis = (H[2][0] * j) + (H[2][1] * i) + (H[2][2] * 1);
            int y = int(((H[0][0] * j) + (H[0][1] * i) + (H[0][2] * 1)) / dis);
            int x = int(((H[1][0] * j) + (H[1][1] * i) + (H[1][2] * 1)) / dis);
            if (x >= 0 && x < INPUT_H && y >= 0 && y < INPUT_W) {
                input2[i][j] = input[1][x][y];
            }
            else {
                input2[i][j] = 0;
            }
        }
    }

    Output = Mat(INPUT_H * 2, INPUT_W, CV_32FC1, 0.0f);
    for (int h = 0; h < INPUT_H; ++h) {
        for (int w = 0; w < INPUT_W; ++w) {
            Output.at<float>(h, w) = input[0][h][w] / 255.0;
            //Output.at<float>(w, h + INPUT_W) = input[1][h][w];
        }
    }
    for (int h = 0; h < INPUT_H; ++h) {
        for (int w = 0; w < INPUT_W; ++w) {
            Output.at<float>(h + INPUT_H, w) = input[1][h][w] / 255.0;
            //Output.at<float>(w, h + INPUT_W) = input[2][h][w];
        }
    }

    /**
    for (int pp = 0; pp < 2; pp++) {
        for (int i = 0; i < MAX_ST; i++) {
            if (keypoint[pp][i].axis[0] == 0) {
                break;
            }
            if (keypoint[pp][i].axis[0] != 0) {
                circle(Output, Point(keypoint[pp][i].axis[0], keypoint[pp][i].axis[1] + pp * INPUT_H), 3, Scalar(200), 1);
            }
            float thita = keypoint[pp][i].angle * 3.141592 / 180;
            if (keypoint[pp][i].axis[0] != 0) {
                arrowedLine(Output,
                    Point(keypoint[pp][i].axis[0] + cos(thita) * 10, keypoint[pp][i].axis[1] + sin(thita) * 10 + pp * INPUT_H),
                    Point(keypoint[pp][i].axis[0], keypoint[pp][i].axis[1] + pp * INPUT_H),
                    Scalar(200), 1, 8, 0, 0.02);
            }
        }
    }


    for (int i = 0; i < 100; i++) {
        arrowedLine(Output,
            Point(saveKnP[i][0], saveKnP[i][1]),
            Point(saveKnP[i][2], saveKnP[i][3] + INPUT_H),
            Scalar(200), 1, 8, 0, 0.02);
    }

    cv::imshow(OutSign.append(to_string(66)), Output);
    //imwrite("output.jpg", Output * 255);

    while (1) {
        waitKey(10);
    }
    **/
    /*
    Mat descriptors_box, descriptors_sence;
    Mat box = imread("Pic/t1.jpg");
    Mat scene = imread("Pic/t2.jpg");


    resize(box, box, Size(INPUT_W, INPUT_H), INTER_NEAREST);
    resize(scene, scene, Size(INPUT_W, INPUT_H), INTER_NEAREST);

    vector<KeyPoint> keypoints_obj, keypoints_sence;

    Ptr<ORB> detector = ORB::create(50,2,4);

    //setUseOptimized(false)
    detector->detectAndCompute(scene, Mat(), keypoints_sence, descriptors_sence);
    START = getTickCount();
    for (int i = 0; i < 200; i++) {
        detector->detectAndCompute(box, Mat(), keypoints_obj, descriptors_box);
    }

    TOTAL = getTickCount() - START;

    std::cout << TOTAL / getTickFrequency() << endl;

    vector<DMatch> matches;

    Ptr<DescriptorMatcher> matcher = makePtr<FlannBasedMatcher>(makePtr<flann::LshIndexParams>(12, 20, 2));

    matcher->match(descriptors_box, descriptors_sence, matches);

    vector<DMatch> goodMatches;
    float maxdist = 0;

    for (unsigned int i = 0; i < matches.size(); ++i) {
        maxdist = max(maxdist, matches[i].distance);
    }

    for (unsigned int i = 0; i < matches.size(); ++i) {
        if (matches[i].distance < maxdist * RATIO)
            goodMatches.push_back(matches[i]);
    }

    Mat dst;

    drawMatches(box, keypoints_obj, scene, keypoints_sence, goodMatches, dst);
    imshow("output", dst);
    waitKey(0);
    
    */
    return 0;
}

//-----------------------------------------------------------------------------------
//[INPUT_W][INPUT_H]
//-----------------------------------------------------------------------------------
/*

void convolution(uchar data2[INPUT_H][INPUT_W], uchar* data1, int H, int W, float k[]) {
    for (int i = 1; i < H - 1; i++) {
        for (int j = 1; j < W - 1; j++) {
            *(data1+i*W+j) = 
                (abs(data2[i - 1][j - 1] * k[0] + data2[i][j - 1] * k[1] + data2[i + 1][j - 1] * k[2] +
                        data2[i - 1][j]     * k[3] + data2[i][j]     * k[4] + data2[i + 1][j]     * k[5] +
                        data2[i - 1][j + 1] * k[6] + data2[i][j + 1] * k[7] + data2[i + 1][j + 1] * k[8]) +
                 abs(data2[i - 1][j - 1] * k[0] + data2[i][j - 1] * k[3] + data2[i + 1][j - 1] * k[6] +
                        data2[i - 1][j]     * k[1] + data2[i][j]     * k[4] + data2[i + 1][j]     * k[7] +
                        data2[i - 1][j + 1] * k[2] + data2[i][j + 1] * k[5] + data2[i + 1][j + 1] * k[8]))/5;
        }
    }
}

uint16_t briefSelect(uchar * input, int H, int W, int thita, int k1[16][2], int k2[16][2], int dis) {
    uint16_t aa = 0;
    for (int k = 0; k < 16; k++) {
        int s1[2] = { k + thita, k + thita + dis };
        for (int k = 0; k < 2; k++) {
            if (s1[k] < 0) {
                s1[k] += 16;
            }
            else if (s1[k] >= 16) {
                s1[k] -= 16;
            }
        }
        float tt0 = *(input + (H + k1[s1[0]][0]) * INPUT_H + (W + k1[s1[0]][1]));
        float tt1 = *(input + (H + k2[s1[1]][0]) * INPUT_H + (W + k2[s1[1]][1]));
        if (abs(tt0 - tt1) < 20) {
            aa |= false << k;
        }
        else {
            aa |= (tt0 < tt1) << k;
        }
    }
    return aa;
}
*/

void initKeyPoint() {
    int keypointO[SLICE_TIMES][SLICE_TIMES] = { 0 };
    String file_Path = "PicB/";
    //file_Path = file_Path.append("000");
    file_Path = file_Path.append(to_string(oopoo));
    file_Path = file_Path.append(".JPG");
    image = imread(file_Path);
    image.convertTo(image, CV_32F);
    resize(image, image, Size(INPUT_W, INPUT_H), INTER_NEAREST);
    cover_to_gray(&image, *input[0], INPUT_H, INPUT_W);

    //描述子BRIEF，在特徵點周圍以一定模式隨機取N個點對，將此點對比對大小的組合
    //能使用描述子來尋找兩張照片的特徵點中，哪裡的特徵點相同。
    vector<Pattern> _pattern;
    const Pattern* pattern0 = (const Pattern*)bit_pattern_31_;
    copy(pattern0, pattern0 + 512, std::back_inserter(_pattern));

    START[4] = getTickCount();

    //fast 在圖像中找出特徵點，並且用區域抑制 (尋找鄰域範圍3x3內是否有相同的特徵點、把畫面切成25x25份，限制每份只能有一個特徵點) 
    //與角點抑制的方式篩選角點
    int oo = 0;
    gaussianBlur1(*input[0], INPUT_H, INPUT_W);
    gaussianBlur2(*input[0], INPUT_H, INPUT_W);
    for (int h = STP * DIS; h < INPUT_H - STP * DIS; h++) {
        unsigned char* ptr = input[0][h] + STP;
        unsigned char curr = 0;
        float ang_curr = 0;

        for (int w = STP * DIS; w < INPUT_W - STP * DIS; w++, ptr++) {
            int v = ptr[0];
            const unsigned char* tab = &threshold_tab[0] - v + 255;
            int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];

            if (d == 0)
                continue;

            d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
            d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
            d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];
            if (d == 0)
                continue;

            d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
            d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
            d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
            d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];

            if (d & 1) {
                int vt = v - 10, count = 0;
                for (int k = 0; k < 25; k++)
                {
                    int x = ptr[pixel[k]];
                    if (x < vt)
                    {
                        if (++count > 8)
                        {
                            curr = (unsigned char)cornerScore(ptr, pixel, 10);
                            // ang_curr[w] = 360-360/16 * (k % 16);
                            break;
                        }
                    }
                    else
                        count = 0;
                }
            }

            if (d & 2) {
                int vt = v + 10, count = 0;
                for (int k = 0; k < 25; k++)
                {
                    int x = ptr[pixel[k]];
                    if (x > vt)
                    {
                        if (++count > 8)
                        {
                            curr = (unsigned char)cornerScore(ptr, pixel, 10);
                            //ang_curr[w] = 360- 360 / 16 * (k % 16);
                            break;
                        }
                    }
                    else
                        count = 0;
                }
            }

            int sh = h * SLICE_TIMES / INPUT_H;
            int sw = w * SLICE_TIMES / INPUT_W;
            input[4][h][w] = 255;
            if (keypointO[sh][sw] == 0) {
                keypoint[oo].axis[0] = w;
                keypoint[oo].axis[1] = h;
                keypoint[oo].response = (float)curr;
                keypoint[oo].angle = ang_curr;
                keypointO[sh][sw] = oo;
                oo += 1;
            }
            else if (keypoint[keypointO[sh][sw]].response < curr) {
                keypoint[keypointO[sh][sw]].axis[0] = w;
                keypoint[keypointO[sh][sw]].axis[1] = h;
                keypoint[keypointO[sh][sw]].response = (float)curr;
                keypoint[keypointO[sh][sw]].angle = ang_curr;
            }
        }
    }

    oo = 0;
    int ptidx = 0;
    const unsigned char* ptr00 = *input[0];
    int r = BLOCK_SIZE / 2;
    //harris 角點分數
    for (ptidx = 0; ptidx < MAX_ST; ptidx++)
    {
        int x0 = keypoint[ptidx].axis[0];
        if (x0 == 0)
            break;

        int y0 = keypoint[ptidx].axis[1];
        const unsigned char* ptr0 = ptr00 + (y0 - r) * INPUT_W + (x0 - r);  //目標點向上r格再向左r格的像素點
        int a = 0, b = 0, c = 0;
        for (int k = 0; k < BLOCK_SIZE * BLOCK_SIZE; k++)
        {
            const unsigned char* ptr = ptr0 + ofs[k];
            int Ix = (ptr[1] - ptr[-1]) * 2 + (ptr[-INPUT_W + 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[INPUT_W - 1]);
            int Iy = (ptr[INPUT_W] - ptr[-INPUT_W]) * 2 + (ptr[INPUT_W - 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[-INPUT_W + 1]);
            a += Ix * Ix;
            b += Iy * Iy;
            c += Ix * Iy;
        }
        keypoint[ptidx].response = ((float)a * b - (float)c * c - HARRIS_K * ((float)a + b) * ((float)a + b)) * scale_sq_sq;
    }

    //IC_Angle
    for (ptidx = 0; ptidx < MAX_ST; ptidx++)
    {
        int x0 = keypoint[ptidx].axis[0];
        if (x0 == 0)
            break;
        const unsigned char* center = (*input[0]) + keypoint[ptidx].axis[1] * INPUT_W + keypoint[ptidx].axis[0];

        int m_01 = 0, m_10 = 0;

        for (int u = -10; u <= 10; ++u)
            m_10 += u * center[u];

        for (int v = 1; v <= 10; ++v)
        {
            int v_sum = 0;
            int d = umax[v];
            for (int u = -d; u <= d; ++u)
            {
                int val_plus = center[u + v * INPUT_W], val_minus = center[u - v * INPUT_W];
                v_sum += (val_plus - val_minus);
                m_10 += u * (val_plus + val_minus);
            }
            m_01 += v * v_sum;
        }
        keypoint[ptidx].angle = fastatan2((float)m_01, (float)m_10);
    }

    //brief
    for (ptidx = 0; ptidx < MAX_ST; ptidx++)
    {
        int x0 = keypoint[ptidx].axis[0];
        if (x0 == 0)
            break;
        float angle = keypoint[ptidx].angle;
        angle *= (float)(3.141592 / 180.f);
        float a = (float)cos(angle), b = (float)sin(angle);

        const unsigned char* center = (*input[0]) + keypoint[ptidx].axis[1] * INPUT_W + keypoint[ptidx].axis[0];
        float x, y;
        int ix, iy;
        Pattern* pattern = &_pattern[0];
        unsigned char* desc = keypoint[ptidx].descriptors;
#if 1  
#define GET_VALUE(idx) \
               (x = (pattern[idx].x*a - pattern[idx].y*b) * DIS, \
                y = (pattern[idx].x*b + pattern[idx].y*a) * DIS, \
                ix = (int)(x + (x >= 0 ? 0.5f : -0.5f)), \
                iy = (int)(y + (y >= 0 ? 0.5f : -0.5f)), \
                *(center + iy*INPUT_W + ix) )
#endif
        for (int i = 0; i < 16; ++i, pattern += 16)//每個特徵描述子長度爲8個字節  
        {
            int t0, t1, val;
            t0 = GET_VALUE(0); t1 = GET_VALUE(1);
            val = (t0 + 2 < t1);
            t0 = GET_VALUE(2); t1 = GET_VALUE(3);
            val |= (t0 + 2 < t1) << 1;
            t0 = GET_VALUE(4); t1 = GET_VALUE(5);
            val |= (t0 + 2 < t1) << 2;
            t0 = GET_VALUE(6); t1 = GET_VALUE(7);
            val |= (t0 + 2 < t1) << 3;
            t0 = GET_VALUE(8); t1 = GET_VALUE(9);
            val |= (t0 + 2 < t1) << 4;
            t0 = GET_VALUE(10); t1 = GET_VALUE(11);
            val |= (t0 + 2 < t1) << 5;
            t0 = GET_VALUE(12); t1 = GET_VALUE(13);
            val |= (t0 + 2 < t1) << 6;
            t0 = GET_VALUE(14); t1 = GET_VALUE(15);
            val |= (t0 + 2 < t1) << 7;

            desc[i] = (unsigned char)val;
        }
#undef GET_VALUE
    }
}

void findKeyPoint(SaveKnP* saveKnP) {
    String file_Path = "PicB/";
    /**
    if (oopoo < 10) {
        file_Path = file_Path.append("000");
        file_Path = file_Path.append(to_string(oopoo));
    }
    else if (oopoo < 100) {
        file_Path = file_Path.append("00");
        file_Path = file_Path.append(to_string(oopoo));
    }
    else if (oopoo < 1000) {
        file_Path = file_Path.append("0");
        file_Path = file_Path.append(to_string(oopoo));
    }
    **/
    file_Path = file_Path.append(to_string(oopoo));
    file_Path = file_Path.append(".JPG");
    image = imread(file_Path);
    image.convertTo(image, CV_32F);
    resize(image, image, Size(INPUT_W, INPUT_H), INTER_NEAREST);
    cover_to_gray(&image, *input[1], INPUT_H, INPUT_W);

    //描述子BRIEF，在特徵點周圍以一定模式隨機取N個點對，將此點對比對大小的組合
    //能使用描述子來尋找兩張照片的特徵點中，哪裡的特徵點相同。
    vector<Pattern> _pattern;
    const Pattern* pattern0 = (const Pattern*)bit_pattern_31_;
    copy(pattern0, pattern0 + 512, std::back_inserter(_pattern));

    //fast 區塊搜索，因為 ESP32 的緩存溢出，所以為了避免掉此問題，分別分區找一個 keypoint (共30x30)，這樣就不用儲存 keypoint 數據，減少緩存占用。 
    int oo = 0;
    connectTimes = 0;
    START[3] = getTickCount();
    const unsigned char* ptr00 = *input[1];
    gaussianBlur1(*input[1], INPUT_H, INPUT_W);
    gaussianBlur2( *input[1], INPUT_H, INPUT_W);

    for (int i = 0; i < MAX_CONNECT; i++) {
        saveR[i] = 0;
        saveD[i] = 0;
    }
    for (int H = 0; H < SLICE_TIMES; H++) {
        int H_size_Start = (int)((float)INPUT_H / 30.0 * H);
        int H_size_Stop = (int)((float)INPUT_H / 30.0 * (H + 1));
        if (H_size_Start < STP * DIS) {
            H_size_Start = STP * DIS;
            if (H_size_Stop < STP * DIS) {
                H_size_Stop = STP * DIS;
            }
        }
        if (H_size_Stop >= INPUT_H - STP * DIS) {
            H_size_Stop = INPUT_H - STP * DIS;
            if (H_size_Start >= INPUT_H - STP * DIS) {
                H_size_Start = INPUT_H - STP * DIS;
            }
        }
        for (int W = 0; W < SLICE_TIMES; W++) {
            int W_size_Start = (int)((float)INPUT_W / 30.0 * W);
            int W_size_Stop = (int)((float)INPUT_W / 30.0 * (W + 1));
            if (W_size_Start < STP * DIS) {
                W_size_Start = STP * DIS;
                if (W_size_Stop < STP * DIS) {
                    W_size_Stop = STP * DIS;
                }
            }
            if (W_size_Stop >= INPUT_W - STP * DIS) {
                W_size_Stop = INPUT_W - STP * DIS;
                if (W_size_Start >= INPUT_W - STP * DIS) {
                    W_size_Start = INPUT_W - STP * DIS;
                }
            }

            Keypoint S_keypoint;
            for (int h = H_size_Start; h < H_size_Stop; h++) {
                unsigned char* ptr = input[1][h] + W_size_Start;
                unsigned char curr = 0;
                for (int w = W_size_Start; w < W_size_Stop; w++, ptr++) {
                    int v = ptr[0];
                    const unsigned char* tab = &threshold_tab[0] - v + 255;
                    int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];

                    if (d == 0)
                        continue;

                    d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
                    d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
                    d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];
                    if (d == 0)
                        continue;

                    d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
                    d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
                    d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
                    d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];


                    if (d & 1) {
                        int vt = v - 10, count = 0;
                        for (int k = 0; k < 25; k++)
                        {
                            int x = ptr[pixel[k]];
                            if (x < vt)
                            {
                                if (++count > 8)
                                {
                                    curr = (unsigned char)cornerScore(ptr, pixel, 10);
                                    break;
                                }
                            }
                            else
                                count = 0;
                        }
                    }

                    if (d & 2) {
                        int vt = v + 10, count = 0;
                        for (int k = 0; k < 25; k++)
                        {
                            int x = ptr[pixel[k]];
                            if (x > vt)
                            {
                                if (++count > 8)
                                {
                                    curr = (unsigned char)cornerScore(ptr, pixel, 10);
                                    break;
                                }
                            }
                            else
                                count = 0;
                        }
                    }

                    if (S_keypoint.response < (float)curr) {
                        S_keypoint.axis[0] = w;
                        S_keypoint.axis[1] = h;
                        S_keypoint.response = (float)curr;
                    }
                }
            }

            int ptidx = 0;
            int r = BLOCK_SIZE / 2;
            if (S_keypoint.axis[0] != 0) {
                input[3][S_keypoint.axis[1]][S_keypoint.axis[0]] = 1;
                //harris 角點分數
                for (ptidx = 0; ptidx < 1; ptidx++)
                {
                    const unsigned char* ptr0 = ptr00 + (S_keypoint.axis[1] - r) * INPUT_W + (S_keypoint.axis[0] - r);  //目標點向上r格再向左r格的像素點
                    int a = 0, b = 0, c = 0;
                    for (int k = 0; k < BLOCK_SIZE * BLOCK_SIZE; k++)
                    {
                        const unsigned char* ptr = ptr0 + ofs[k];
                        int Ix = (ptr[1] - ptr[-1]) * 2 + (ptr[-INPUT_W + 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[INPUT_W - 1]);
                        int Iy = (ptr[INPUT_W] - ptr[-INPUT_W]) * 2 + (ptr[INPUT_W - 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[-INPUT_W + 1]);
                        a += Ix * Ix;
                        b += Iy * Iy;
                        c += Ix * Iy;
                    }
                    S_keypoint.response = ((float)a * b - (float)c * c - HARRIS_K * ((float)a + b) * ((float)a + b)) * scale_sq_sq;
                }

                //IC_Angle
                for (ptidx = 0; ptidx < 1; ptidx++)
                {
                    const unsigned char* center = (*input[1]) + S_keypoint.axis[1] * INPUT_W + S_keypoint.axis[0];

                    int m_01 = 0, m_10 = 0;

                    for (int u = -10; u <= 10; ++u)
                        m_10 += u * center[u];

                    for (int v = 1; v <= 10; ++v)
                    {
                        int v_sum = 0;
                        int d = umax[v];
                        for (int u = -d; u <= d; ++u)
                        {
                            int val_plus = center[u + v * INPUT_W], val_minus = center[u - v * INPUT_W];
                            v_sum += (val_plus - val_minus);
                            m_10 += u * (val_plus + val_minus);
                        }
                        m_01 += v * v_sum;
                    }
                    S_keypoint.angle = fastatan2((float)m_01, (float)m_10);
                }

                START[3] = getTickCount();
                //brief
                for (ptidx = 0; ptidx < 1; ptidx++)
                {
                    float angle = S_keypoint.angle;
                    angle *= (float)(3.141592 / 180.f);
                    float a = (float)cos(angle), b = (float)sin(angle);

                    const unsigned char* center = (*input[1]) + S_keypoint.axis[1] * INPUT_W + S_keypoint.axis[0];
                    float x, y;
                    int ix, iy;
                    Pattern* pattern = &_pattern[0];
                    unsigned char* desc = S_keypoint.descriptors;
#if 1  
#define GET_VALUE(idx) \
               (x = (pattern[idx].x*a - pattern[idx].y*b) * DIS, \
                y = (pattern[idx].x*b + pattern[idx].y*a) * DIS, \
                ix = (int)(x + (x >= 0 ? 0.5f : -0.5f)), \
                iy = (int)(y + (y >= 0 ? 0.5f : -0.5f)), \
                *(center + iy*INPUT_W + ix) )
#endif
                    for (int i = 0; i < 16; ++i, pattern += 16)//每個特徵描述子長度爲8個字節  
                    {
                        int t0, t1, val;
                        t0 = GET_VALUE(0); t1 = GET_VALUE(1);
                        val = (t0 + 2 < t1);
                        t0 = GET_VALUE(2); t1 = GET_VALUE(3);
                        val |= (t0 + 2 < t1) << 1;
                        t0 = GET_VALUE(4); t1 = GET_VALUE(5);
                        val |= (t0 + 2 < t1) << 2;
                        t0 = GET_VALUE(6); t1 = GET_VALUE(7);
                        val |= (t0 + 2 < t1) << 3;
                        t0 = GET_VALUE(8); t1 = GET_VALUE(9);
                        val |= (t0 + 2 < t1) << 4;
                        t0 = GET_VALUE(10); t1 = GET_VALUE(11);
                        val |= (t0 + 2 < t1) << 5;
                        t0 = GET_VALUE(12); t1 = GET_VALUE(13);
                        val |= (t0 + 2 < t1) << 6;
                        t0 = GET_VALUE(14); t1 = GET_VALUE(15);
                        val |= (t0 + 2 < t1) << 7;

                        desc[i] = (unsigned char)val;
                    }
#undef GET_VALUE
                }
                TOTAL[3] += getTickCount() - START[3];

                int keyTo[3] = { 0, 1000};

                connect_keypoint(S_keypoint, keyTo, 2);

                if (keyTo[1] < MATCHING_FILTER) {
                    if (connectTimes < MAX_CONNECT) {
                        saveKnP[connectTimes].b_X = keypoint[keyTo[0]].axis[0];
                        saveKnP[connectTimes].b_Y = keypoint[keyTo[0]].axis[1];
                        saveKnP[connectTimes].n_X = S_keypoint.axis[0];
                        saveKnP[connectTimes].n_Y = S_keypoint.axis[1];
                        saveR[connectTimes] = keyTo[1];
                        saveD[connectTimes] = sqrt((S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) * (S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) +
                                          (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]) * (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]));
                        keypoint[keyTo[0]].po = true;
                    }
                    else {
                        int j[2] = { -1,0 };
                        for (int i = 0; i < MAX_CONNECT; i++) {
                            if (j[0] < saveR[i] && saveR[i] > keyTo[1]) {
                                j[0] = saveR[i];
                                j[1] = i;
                            }
                        }
                        if (j[0] >= 0) {
                            saveKnP[j[1]].b_X = keypoint[keyTo[0]].axis[0];
                            saveKnP[j[1]].b_Y = keypoint[keyTo[0]].axis[1];
                            saveKnP[j[1]].n_X = S_keypoint.axis[0];
                            saveKnP[j[1]].n_Y = S_keypoint.axis[1];
                            saveR[connectTimes] = keyTo[1];
                            saveD[connectTimes] = sqrt((S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) * (S_keypoint.axis[0] - keypoint[keyTo[0]].axis[0]) +
                                (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]) * (S_keypoint.axis[1] - keypoint[keyTo[0]].axis[1]));
                        }
                    }
                    connectTimes += 1;
                }
            }
        }
    }

    cout << "Target_pic: " << tg_oopoo << " , Connect_times:" << connectTimes << " , ";

    //對brief配對數進行排序
    if (connectTimes > MAX_CONNECT) {
        connectTimes = MAX_CONNECT;
    }
    for (int i = 0; i < connectTimes; i++) {
        for (int j = i; j < connectTimes; j++) {
            if (saveR[i] > saveR[j]) {
                swap(saveKnP[i], saveKnP[j]);
                swap(saveR[i], saveR[j]);
                swap(saveD[i], saveD[j]);
            }
        }
    }

    float avg_Dis = 0, SD = 0;
    for (int i = 0; i < connectTimes; i++) {
        avg_Dis += saveD[i];
    }
    if (avg_Dis != 0) {
        avg_Dis /= connectTimes;
        for (int i = 0; i < connectTimes; i++) {
            SD += (saveD[i] - avg_Dis) * (saveD[i] - avg_Dis);
        }

        int kl = 0;
        if (SD != 0) {
            SD /= float(connectTimes); SD = sqrt(SD);

            
            for (int p = 0; p < connectTimes; p++) {
                if (saveD[p] > avg_Dis + (SD * 1.5 + 1) || saveD[p] < avg_Dis - (SD * 1.5 + 1)) {
                    saveKnPi[kl][0] = saveKnP[p].b_X;
                    saveKnPi[kl][1] = saveKnP[p].b_Y;
                    saveKnPi[kl][2] = saveKnP[p].n_X;
                    saveKnPi[kl][3] = saveKnP[p].n_Y;

                    swap(saveKnP[p], saveKnP[connectTimes - kl - 1]);
                    swap(saveR[p], saveR[connectTimes - kl - 1]);
                    swap(saveD[p], saveD[connectTimes - kl - 1]);

                    saveR[p] = 10000;
                    kl += 1;
                }
            }
        }
        connectTimes -= kl;
    }

    float sec, sc;
    int ths;
    const int plac[8][2] = { {0,0},{0,INPUT_H},{INPUT_W,0},{INPUT_W,INPUT_H},{0,0},{0,INPUT_H},{INPUT_W,0},{INPUT_W,INPUT_H} };
    //篩選出距離相差最大的8個點，並且排到最前面去優先計算，這樣能夠更精準的求出單應矩陣
    for (int i = 0; i < 8; i++) {
        ths = 0;
        sec = 100000000;
        for (int j = i; j < connectTimes; j++) {
            if (float(saveR[j]) > float(saveR[i]) * 1.5) continue;
            sc = pow(plac[i][0] - saveKnP[j].n_X, 2) * 0.9 + pow(plac[i][1] - saveKnP[j].n_Y, 2) * 1.6;
            if (sc < sec) {
                ths = j;
                sec = sc;
            }
        }
        if (ths != 0) {
            std::swap(saveKnP[i], saveKnP[ths]);
            std::swap(saveR[i], saveR[ths]);
            std::swap(saveD[i], saveD[ths]);
        }
    }

}

void cover_to_gray(Mat* Input, uchar* Output, int H, int W) {
    vector<Mat> channels;
    split(*Input, channels);

    for (int h = 0; h < H; ++h) {
        for (int w = 0; w < W; ++w) {
            *(Output + h * W + w) = channels[0].at<float>(h, w) * 0.114 +
                channels[1].at<float>(h, w) * 0.587 +
                channels[2].at<float>(h, w) * 0.299;
        }
    }
}

void connect_keypoint(Keypoint target, int* keyTo, float range)
{
    for (int i = 0; i < 900; i++) {
        if (keypoint[i].axis[0] == 0)
            break;
        if (keypoint[i].response < target.response / range)
            continue;
        if (keypoint[i].response > target.response * range)
            continue;
        //input[1][(*(keypoint + i)).axis[0]][(*(keypoint + i)).axis[1]] = 1;
        int answ = 0;
        for (int l = 0; l < 16; l++) {
            unsigned char app = target.descriptors[l] ^ keypoint[i].descriptors[l];
            while (app != 0) {
                answ += 1;
                app &= app - 1;
            }
            if (answ > MATCHING_FILTER) break;
        }
        if (answ < keyTo[1]) {
            *keyTo = i;
            *(keyTo + 1) = answ;
        }
    }
}

int cornerScore(unsigned char* ptr, int pixel[], int threshold) {
    const int N = 25;
    int k, v = ptr[0];
    short d[N];
    for (k = 0; k < N; k++)
        d[k] = (short)(v - ptr[pixel[k]]);

    int a0 = threshold;
    for (k = 0; k < 16; k += 2)
    {
        int a = std::min((int)d[k + 1], (int)d[k + 2]);
        a = std::min(a, (int)d[k + 3]);

        if (a <= a0)
            continue;

        a = std::min(a, (int)d[k + 4]);
        a = std::min(a, (int)d[k + 5]);
        a = std::min(a, (int)d[k + 6]);
        a = std::min(a, (int)d[k + 7]);
        a = std::min(a, (int)d[k + 8]);
        a0 = std::max(a0, std::min(a, (int)d[k]));
        a0 = std::max(a0, std::min(a, (int)d[k + 9]));
    }

    int b0 = -a0;
    for (k = 0; k < 16; k += 2)
    {
        int b = std::max((int)d[k + 1], (int)d[k + 2]);
        b = std::max(b, (int)d[k + 3]);
        b = std::max(b, (int)d[k + 4]);
        b = std::max(b, (int)d[k + 5]);
        if (b >= b0)
            continue;
        b = std::max(b, (int)d[k + 6]);
        b = std::max(b, (int)d[k + 7]);
        b = std::max(b, (int)d[k + 8]);

        b0 = std::min(b0, std::max(b, (int)d[k]));
        b0 = std::min(b0, std::max(b, (int)d[k + 9]));
    }

    threshold = -b0 - 1;

    return threshold;
}
/*
//高斯模糊水平方向
void gaussianBlur1(uint8_t* input, int H, int W) {
    int i, j, p, s1, s2;
    for (i = 1; i < H - 1; i++) {
        p = i * W;
        s1 = input[p - 1];
        for (j = 1; j < W - 1; j++) {
            p = i * W + j;
            s2 = input[p];
            input[p] = 0.25f * (s1 + input[p + 1]) + 0.5f * input[p];
            s1 = s2;
        }
    }
}

//高斯模糊垂直方向
void gaussianBlur2(uint8_t* input, int H, int W) {
    int i, j, p, s1, s2;
    for (i = 1; i < W - 1; i++) {
        p = i * W;
        s1 = input[p - 1 * W];
        for (j = 1; j < H - 1; j++) {
            p = i + j * W;
            s2 = input[p];
            input[p] = 0.25f * (s1 + input[p + 1 * W]) + 0.5f * input[p];
            s1 = s2;
        }
    }
}
*/

void gaussianBlur1(unsigned char* output, int H, int W) {
    int i, j, p, s1, s2, s3;
    for (i = 2; i < H-2; i++) {
        p = i * W;
        s1 = output[p - 2];
        s2 = output[p - 1];
        for (j = 2; j < W-2; j++) {
            p = i * W + j;
            s3 = output[p];
            output[p] =
                unsigned char(0.0625f * s1 +
                0.25f * s2 +
                0.375f * output[p] +
                0.25f * output[p+1] +
                0.0625f * output[p+2]);
            s1 = s2;
            s2 = s3;
        }
    }
}

void gaussianBlur2(unsigned char* output, int H, int W) {
    int i, j, p, s1, s2, s3;
    for (i = 2; i < W - 2; i++) {
        p = i * W;
        s1 = output[p - 2 * W];
        s2 = output[p - 1 * W];
        for (j = 2; j < H - 2; j++) {
            p = i + j * W;
            s3 = output[p];
            output[p] =
                unsigned char(0.0625f * s1 +
                0.25f * s2 +
                0.375f * s3 +
                0.25f * output[p + 1 * W] +
                0.0625f * output[p + 2 * W]);
            s1 = s2;
            s2 = s3;
        }
    }
}

void Keypoint_Sort(Keypoint* keypoint) {
    for (int i = 0; i < 2000; i++) {
        if (keypoint[i].axis[0] == 0)
            break;
        for (int j =i; j < 2000; j++) {
            if (keypoint[j].axis[0] == 0)
                break;
            if (keypoint[i].response < keypoint[j].response) {
                swap(keypoint[i], keypoint[j]);
            }
        }
    }
}

static const float atan2_p1 = 0.9997878412794807f * (float)(180 / CV_PI);
static const float atan2_p3 = -0.3258083974640975f * (float)(180 / CV_PI);
static const float atan2_p5 = 0.1555786518463281f * (float)(180 / CV_PI);
static const float atan2_p7 = -0.04432655554792128f * (float)(180 / CV_PI);

float fastatan2(float y, float x)
{
    float ax = std::abs(x), ay = std::abs(y);//首先不分象限，求得一个锐角角度
    float a, c, c2;
    if (ax >= ay)
    {
        c = ay / (ax + (float)DBL_EPSILON);
        c2 = c * c;
        a = (((atan2_p7 * c2 + atan2_p5) * c2 + atan2_p3) * c2 + atan2_p1) * c;
    }
    else
    {
        c = ax / (ay + (float)DBL_EPSILON);
        c2 = c * c;
        a = 90.f - (((atan2_p7 * c2 + atan2_p5) * c2 + atan2_p3) * c2 + atan2_p1) * c;
    }
    if (x < 0)//锐角求出后，根据x和y的正负性确定向量的方向，即角度。
        a = 180.f - a;
    if (y < 0)
        a = 360.f - a;
    return a;
}

int svd(float * matSrc, float * matD, float * matU, float * matVt, const int tm , const int tn)
{
    int m = tm;
    int n = tn;
    bool at = false;
    if (m < n) {
        std::swap(m, n);
        at = true;
    }

    float* tmp_u = new float[m * m];
    float* tmp_v = new float[n * n];
    float* tmp_a = new float[m * n];
    float* tmp_a_ = new float[m * m];
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            tmp_u[i * n + j]  = matU[i * n + j];
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            tmp_v[i * n + j] = matVt[i * n + j];
        }
    }

    if (!at)
    {
        int k = 0;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++,k++)
                tmp_a[k] = matSrc[j * n + i];
    }
    else {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                tmp_a[i * n + j] = matSrc[i * n + j];
    }

    if (m == n) {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                tmp_a_[i * n + j] = tmp_a[i * n + j];
    }
    else {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                if (j >= n)
                    tmp_a_[i * n + j] = 0;
                else
                    tmp_a_[i * n + j] = tmp_a[i * n + j];
    }

    jacobiSVD(tmp_a_, matD, tmp_v, m, n);

    if (!at) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                matVt[i * n + j] = tmp_v[i * n + j];
        for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                matU[j * m + i] = tmp_a_[i * m + j];
    }
    else {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                matU[i * n + j] = tmp_v[j * n + i];
        for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                matVt[i * m + j] = tmp_a_[i * m + j];
    }

    return 0;
}

void jacobiSVD(float * At, float * _W,float * Vt, const int m, const int n)
{
    int p = 0;
    double minval = FLT_MIN;
    auto eps = (float)(FLT_EPSILON * 2);
    const int n1 = m;

    double* W = new double[n];
    for (int i = 0; i < n; i++)
        W[i] = 0;

    for (int i = 0; i < n; i++) {
        double sd{ 0. };
        for (int k = 0; k < m; k++) {
            float t = At[i * m + k];
            sd += (double)t * t;
        }
        W[i] = sd;

        for (int k = 0; k < n; k++)
            Vt[i*n+k] = 0;
        Vt[i*n+i] = 1;
    }

    int max_iter = std::max(m, 30);
    for (int iter = 0; iter < max_iter; iter++) {
        bool changed = false;
        float c, s;

        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                float* Ai = At + i * m, * Aj = At + j * m;
                double a = W[i], p = 0, b = W[j];

                for (int k = 0; k < m; k++)
                    p += (double)Ai[k] * Aj[k];

                if (std::abs(p) <= eps * std::sqrt((double)a * b))
                    continue;

                p *= 2;
                double beta = a - b, gamma = hypot((double)p, beta);
                if (beta < 0) {
                    double delta = (gamma - beta) * 0.5;
                    s = (float)std::sqrt(delta / gamma);
                    c = (float)(p / (gamma * s * 2));
                }
                else {
                    c = (float)std::sqrt((gamma + beta) / (gamma * 2));
                    s = (float)(p / (gamma * c * 2));
                }

                a = b = 0;
                for (int k = 0; k < m; k++) {
                    float t0 = c * Ai[k] + s * Aj[k];
                    float t1 = -s * Ai[k] + c * Aj[k];
                    Ai[k] = t0; Aj[k] = t1;

                    a += (double)t0 * t0; b += (double)t1 * t1;
                }
                W[i] = a; W[j] = b;

                changed = true;

                float* Vi = Vt+i*n, * Vj = Vt+j*n;

                for (int k = 0; k < n; k++) {
                    float t0 = c * Vi[k] + s * Vj[k];
                    float t1 = -s * Vi[k] + c * Vj[k];
                    Vi[k] = t0; Vj[k] = t1;
                }
            }
        }
        if (!changed)
            break;
    }

    for (int i = 0; i < n; i++) {
        double sd{ 0. };
        for (int k = 0; k < m; k++) {
            float t = At[i * m + k];
            sd += (double)t * t;
        }
        W[i] = std::sqrt(sd);
    }

    for (int i = 0; i < n - 1; i++) {
        int j = i;
        for (int k = i + 1; k < n; k++) {
            if (W[j] < W[k])
                j = k;
        }
        if (i != j) {
            std::swap(W[i], W[j]);

            for (int k = 0; k < m; k++)
                std::swap(At[i * m + k], At[j * m + k]);

            for (int k = 0; k < n; k++)
                std::swap(Vt[i * m + k], Vt[j * m + k]);
        }
    }

    for (int i = 0; i < n; i++)
        _W[i] = (float)W[i];

    srand(time(nullptr));

    for (int i = 0; i < n1; i++) {
        double sd = i < n ? W[i] : 0;

        for (int ii = 0; ii < 100 && sd <= minval; ii++) {
            // if we got a zero singular value, then in order to get the corresponding left singular vector
            // we generate a random vector, project it to the previously computed left singular vectors,
            // subtract the projection and normalize the difference.
            const auto val0 = (float)(1. / m);
            for (int k = 0; k < m; k++) {
                unsigned long int rng = rand() % 4294967295; // 2^32 - 1
                float val = (rng & 256) != 0 ? val0 : -val0;
                At[i * m + k] = val;
            }
            for (int iter = 0; iter < 2; iter++) {
                for (int j = 0; j < i; j++) {
                    sd = 0;
                    for (int k = 0; k < m; k++)
                        sd += At[i * m + k] * At[j * m + k];
                    float asum = 0;
                    for (int k = 0; k < m; k++) {
                        auto t = (float)(At[i * m + k] - sd * At[j * m + k]);
                        At[i * m + k] = t;
                        asum += std::abs(t);
                    }
                    asum = asum > eps * 100 ? 1 / asum : 0;
                    for (int k = 0; k < m; k++)
                        At[i * m + k] *= asum;
                }
            }

            sd = 0;
            for (int k = 0; k < m; k++) {
                float t = At[i * m + k];
                sd += (double)t * t;
            }
            sd = std::sqrt(sd);
        }

        float s = (float)(sd > minval ? 1 / sd : 0.);
        for (int k = 0; k < m; k++)
            At[i * m + k] *= s;
    }
}

void inv(float* In_mat, float* Out_mat, const int n) {
    float* cb_In_mat = new float[n * n];
    for (int i = 0; i < n * n; i++) {
        cb_In_mat[i] = In_mat[i];
    }

    //to 倒三角矩陣
    for (int i = 0; i < n; i++) {
        float p = cb_In_mat[i * n + i];
        for (int j = 0; j < n; j++)
            cb_In_mat[i * n + j] /= p;
        for (int j = 0; j < i + 1; j++)
            Out_mat[i * n + j] /= p;
        for (int j = i + 1; j < n; j++) {
            float d = cb_In_mat[j * n + i] / cb_In_mat[i * n + i];
            for (int k = 0; k < i + 1; k++)
                Out_mat[j * n + k] -= d * Out_mat[i * n + k];
            for (int k = i; k < n; k++)
                cb_In_mat[j * n + k] -= cb_In_mat[i * n + k] * d;
        }
    }

    //to 單位矩陣
    for (int i = n - 1; i > 0; i--) {
        for (int j = i; j > 0; j--) {
            for (int k = 0; k < 3; k++)
                Out_mat[(j - 1) * n + k] -= cb_In_mat[(j - 1) * n + i] * Out_mat[i * n + k];
            cb_In_mat[(j - 1) * n + i] = 0;
        }
    }
}

void matMult(float* In_mat1, float* In_mat2, float* Out_mat, const int m, const int n, const int o) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < o; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += In_mat1[k + i * n] * In_mat2[k * o + j];
            }
            Out_mat[i * o + j] = sum;
        }
    }
}

void print_matrix(const vector<vector<float>>& vec) {
    if (vec.empty()) {
        return;
    }
    for (auto row : vec) {
        if (row.empty()) {
            return;
        }
        for (auto elem : row) {
            cout << elem << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

void print_array(const float * vec, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << vec[i*n+j] << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

void print_mat(const cv::Mat vec) {
    int n = vec.cols;
    int m = vec.rows;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << vec.at<float>(i,j) << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

bool reconstructH1(float* H21, float* K, SaveKnP * saveKnP)
{
    float invK[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
    float* B = new float[3 * 3];
    float* A = new float[3 * 3];
    inv(K, *invK, 3);

    matMult(*invK, H21, B, 3, 3, 3);
    matMult(B, K, A, 3, 3, 3);

    float w[3][1] = { 0 };
    float U[3][3] = { 0 };
    float Vt[3][3] = { 0 };
    float V[3][3] = { 0 };

    svd(A, *w, *U, *Vt, 3, 3);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            V[i][j] = Vt[j][i];
        }
    }

    float s = (U[0][0] * U[1][1] * U[2][2] + U[0][1] * U[1][2] * U[2][0] + U[0][2] * U[1][0] * U[2][1] -
        U[0][2] * U[1][1] * U[2][0] - U[0][1] * U[1][0] * U[2][2] - U[0][0] * U[1][2] * U[2][1]) *
        (Vt[0][0] * Vt[1][1] * Vt[2][2] + Vt[0][1] * Vt[1][2] * Vt[2][0] + Vt[0][2] * Vt[1][0] * Vt[2][1] -
            Vt[0][2] * Vt[1][1] * Vt[2][0] - Vt[0][1] * Vt[1][0] * Vt[2][2] - Vt[0][0] * Vt[1][2] * Vt[2][1]);

    float d1 = w[0][0];
    float d2 = w[1][0];
    float d3 = w[2][0];

    if (d1 / d2 < 1.00001 || d2 / d3 < 1.00001)
    {
        return false;
    }

    float vR[8][3][3] = { 0 };
    float vt[8][3][1] = { 0 };
    float vn[8][3][1] = { 0 };

    float aux1 = sqrt((d1 * d1 - d2 * d2) / (d1 * d1 - d3 * d3));
    float aux3 = sqrt((d2 * d2 - d3 * d3) / (d1 * d1 - d3 * d3));
    float x1[] = { aux1,aux1,-aux1,-aux1 };
    float x3[] = { aux3,-aux3,aux3,-aux3 };

    float aux_stheta = sqrt((d1 * d1 - d2 * d2) * (d2 * d2 - d3 * d3)) / ((d1 + d3) * d2);

    float ctheta = (d2 * d2 + d1 * d3) / ((d1 + d3) * d2);
    float stheta[] = { aux_stheta, -aux_stheta, -aux_stheta, aux_stheta };

    for (int i = 0; i < 4; i++)
    {
        float Rp[3][3] = { {ctheta,0,-stheta[i]},{0,1,0},{stheta[i],0,ctheta} };
        float R[3][3] = { 0 };

        matMult(*U, *Rp, B, 3, 3, 3);
        matMult(B, *Vt, *R, 3, 3, 3);
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                vR[i][j][k] = R[j][k] * s;
            }
        }

        float tp[3][1] = { {x1[i] * (d1 - d3)}, {0} , {-x3[i] * (d1 - d3)} };
        float t[3][1] = { 0 };

        matMult(*U, *tp, *t, 3, 3, 1);
        float norm = sqrt(t[0][0] * t[0][0] + t[1][0] * t[1][0] + t[2][0] * t[2][0]) ;
        vt[i][0][0] = t[0][0];
        vt[i][1][0] = t[1][0];
        vt[i][2][0] = t[2][0];

        float np[3][1] = { {x1[i]}, {0}, {x3[i]} };
        float n[3][1] = { 0 };

        matMult(*V, *np, *n, 3, 3, 1);
        if (n[2][0] < 0)
            for (int i = 0; i < 3; i++)
                n[i][0] = -n[i][0];
        vn[i][0][0] = n[0][0];
        vn[i][1][0] = n[1][0];
        vn[i][2][0] = n[2][0];
        //cout << "----------------" << endl;
        //print_array(*vR[i], 3, 3);
        //print_array(*vt[i], 1, 3);
        //print_array(*vn[i], 1, 3);
        xp = fastatan2(vR[i][2][1], vR[i][2][2]);
        yp = fastatan2(-vR[i][2][0], sqrt(vR[i][2][1] * vR[i][2][1] + vR[i][2][2] * vR[i][2][2]));
        zp = fastatan2(vR[i][1][0], vR[i][0][0]);
        if (xp > 180) xp -= 360;
        if (yp > 180) yp -= 360;
        if (zp > 180) zp -= 360;
        if (i == 0) {
            cout  << xp << " , " << yp << " , " << zp << " , " << endl;
        }

        if (i == 0) {
            xt = vt[i][0][0];
            yt = vt[i][1][0];
            zt = vt[i][2][0];
        }
    }

    float aux_sphi = sqrt((d1 * d1 - d2 * d2) * (d2 * d2 - d3 * d3)) / ((d1 - d3) * d2);

    float cphi = (d1 * d3 - d2 * d2) / ((d1 - d3) * d2);
    float sphi[] = { aux_sphi, -aux_sphi, -aux_sphi, aux_sphi };

    for (int i = 0; i < 4; i++)
    {
        float Rp[3][3] = { {cphi,0,sphi[i]},{0,1,0},{sphi[i],0,-cphi} };
        float R[3][3] = { 0 };

        matMult(*U, *Rp, B, 3, 3, 3);
        matMult(B, *Vt, *R, 3, 3, 3);
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                vR[i+4][j][k] = R[j][k] * s;

        float tp[3][1] = { {x1[i] * (d1 + d3)}, {0} , {x3[i] * (d1 + d3)} };
        float t[3][1] = { 0 };

        matMult(*U, *tp, *t, 3, 3, 1);
        float norm = sqrt(t[0][0] * t[0][0] + t[1][0] * t[1][0] + t[2][0] * t[2][0]);
        vt[i + 4][0][0] = t[0][0];
        vt[i + 4][1][0] = t[1][0];
        vt[i + 4][2][0] = t[2][0];

        float np[3][1] = { {x1[i]}, {0}, {x3[i]} };
        float n[3][1] = { 0 };

        matMult(*V, *np, *n, 3, 3, 1);
        if (n[2][0] < 0)
            for (int i = 0; i < 3; i++)
                n[i][0] = -n[i][0];
        vn[i + 4][0][0] = n[0][0];
        vn[i + 4][1][0] = n[1][0];
        vn[i + 4][2][0] = n[2][0];
        //cout << "----------------" << endl;
        //print_array(*vR[i+4], 3, 3);
        //print_array(*vt[i+4], 1, 3);
        //print_array(*vn[i+4], 1, 3);
    }

    return false;
}

bool reconstructH(cv::Mat &H21, cv::Mat &K, SaveKnP* saveKnP)
{
    // We recover 8 motion hypotheses using the method of Faugeras et al.
    // Motion and structure from motion in a piecewise planar environment.
    // International Journal of Pattern Recognition and Artificial Intelligence, 1988

    // 因为特征点是图像坐标系，所以讲H矩阵由相机坐标系换算到图像坐标系
    cv::Mat invK = K.inv();
    cv::Mat A = invK*H21*K;

    cv::Mat U,w,Vt,V;
    cv::SVD::compute(A,w,U,Vt,cv::SVD::FULL_UV);
    V=Vt.t();

    float s = cv::determinant(U)*cv::determinant(Vt);

    float d1 = w.at<float>(0);
    float d2 = w.at<float>(1);
    float d3 = w.at<float>(2);

    // SVD分解的正常情况是特征值降序排列
    if(d1/d2<1.00001 || d2/d3<1.00001)
    {
        return false;
    }

    vector<cv::Mat> vR, vt, vn;
    vR.reserve(8);
    vt.reserve(8);
    vn.reserve(8);

    //n'=[x1 0 x3] 4 posibilities e1=e3=1, e1=1 e3=-1, e1=-1 e3=1, e1=e3=-1
    // 法向量n'= [x1 0 x3] 对应ppt的公式17
    float aux1 = sqrt((d1*d1-d2*d2)/(d1*d1-d3*d3));
    float aux3 = sqrt((d2*d2-d3*d3)/(d1*d1-d3*d3));
    float x1[] = {aux1,aux1,-aux1,-aux1};
    float x3[] = {aux3,-aux3,aux3,-aux3};

    //case d'=d2
    // 计算ppt中公式19
    float aux_stheta = sqrt((d1*d1-d2*d2)*(d2*d2-d3*d3))/((d1+d3)*d2);

    float ctheta = (d2*d2+d1*d3)/((d1+d3)*d2);
    float stheta[] = {aux_stheta, -aux_stheta, -aux_stheta, aux_stheta};

    // 计算旋转矩阵 R‘，计算ppt中公式18
    //      | ctheta      0   -aux_stheta|       | aux1|
    // Rp = |    0        1       0      |  tp = |  0  |
    //      | aux_stheta  0    ctheta    |       |-aux3|

    //      | ctheta      0    aux_stheta|       | aux1|
    // Rp = |    0        1       0      |  tp = |  0  |
    //      |-aux_stheta  0    ctheta    |       | aux3|

    //      | ctheta      0    aux_stheta|       |-aux1|
    // Rp = |    0        1       0      |  tp = |  0  |
    //      |-aux_stheta  0    ctheta    |       |-aux3|

    //      | ctheta      0   -aux_stheta|       |-aux1|
    // Rp = |    0        1       0      |  tp = |  0  |
    //      | aux_stheta  0    ctheta    |       | aux3|
    for(int i=0; i<4; i++)
    {
        cv::Mat Rp=cv::Mat::eye(3,3,CV_32F);
        Rp.at<float>(0,0)=ctheta;
        Rp.at<float>(0,2)=-stheta[i];
        Rp.at<float>(2,0)=stheta[i];
        Rp.at<float>(2,2)=ctheta;

        cv::Mat R = s*U*Rp*Vt;
        vR.push_back(R);

        cv::Mat tp(3,1,CV_32F);
        tp.at<float>(0)=x1[i];
        tp.at<float>(1)=0;
        tp.at<float>(2)=-x3[i];
        tp*=d1-d3;

        // 这里虽然对t有归一化，并没有决定单目整个SLAM过程的尺度
        // 因为CreateInitialMapMonocular函数对3D点深度会缩放，然后反过来对 t 有改变
        cv::Mat t = U*tp;
        cv::Mat ui = t;
        vt.push_back(t/cv::norm(t));

        cv::Mat np(3,1,CV_32F);
        np.at<float>(0)=x1[i];
        np.at<float>(1)=0;
        np.at<float>(2)=x3[i];

        cv::Mat n = V*np;
        if(n.at<float>(2)<0)
            n=-n;
        vn.push_back(n);
        //for (int i = 0; i < 1; i++) {
        //    for (int j = 0; j < 3; j++) {
        //        cout << vn[0].at<float>(i, j) << " , ";
        //    }
        //    cout << endl;
        //}

        //cout << tp.at<float>(0) << " , " << tp.at<float>(1) << " , " << tp.at<float>(2) << endl;
        //cout << ui.at<float>(0) << " , " << ui.at<float>(1) << " , " << ui.at<float>(2) << endl;
        //if (i == 1) {
        //    cout << n.at<float>(0) << " , " << n.at<float>(1) << " , " << n.at<float>(2) << " , " << endl;
        //}
    }

    //case d'=-d2
    // 计算ppt中公式22
    float aux_sphi = sqrt((d1*d1-d2*d2)*(d2*d2-d3*d3))/((d1-d3)*d2);

    float cphi = (d1*d3-d2*d2)/((d1-d3)*d2);
    float sphi[] = {aux_sphi, -aux_sphi, -aux_sphi, aux_sphi};

    // 计算旋转矩阵 R‘，计算ppt中公式21
    for(int i=0; i<4; i++)
    {
        cv::Mat Rp=cv::Mat::eye(3,3,CV_32F);
        Rp.at<float>(0,0)=cphi;
        Rp.at<float>(0,2)=sphi[i];
        Rp.at<float>(1,1)=-1;
        Rp.at<float>(2,0)=sphi[i];
        Rp.at<float>(2,2)=-cphi;

        cv::Mat R = s*U*Rp*Vt;
        vR.push_back(R);

        cv::Mat tp(3,1,CV_32F);
        tp.at<float>(0)=x1[i];
        tp.at<float>(1)=0;
        tp.at<float>(2)=x3[i];
        tp*=d1+d3;

        cv::Mat t = U*tp;
        cv::Mat ui = t;
        vt.push_back(t/cv::norm(t));

        cv::Mat np(3,1,CV_32F);
        np.at<float>(0)=x1[i];
        np.at<float>(1)=0;
        np.at<float>(2)=x3[i];

        cv::Mat n = V*np;
        if (n.at<float>(2) < 0)
            n = -n;
        //if (i == 1) {
        //    cout << ui.at<float>(0) << " , " << ui.at<float>(1) << " , " << ui.at<float>(2) << " , " << endl;
        //}
    }
}

bool reconstructF1(float* F21, float* K, SaveKnP* saveKnP)
{
    float Kt[3][3] = {0};
    float* B = new float[3 * 3];
    float* E21 = new float[3 * 3];
    for (int i = 0; i < 9; i++)
        Kt[i % 3][int(i / 3)] = K[i];
    matMult(*Kt, F21, B, 3, 3, 3);
    matMult(B, K, E21, 3, 3, 3);

    float w[3][1] = { 0 };
    float u[3][3] = { 0 };
    float vt[3][3] = { 0 };

    float R1[3][3] = { 0 };
    float R2[3][3] = { 0 };
    float t[3][1] = { 0 };
    float t1[3] = { 0 };
    float t2[3] = { 0 };

    svd(E21, *w, *u, *vt, 3, 3);

    t[0][0] = u[0][2];
    t[1][0] = u[1][2];
    t[2][0] = u[2][2];
    float s = sqrt(t[0][0] * t[0][0] + t[1][0] * t[1][0] + t[2][0] * t[2][0]);

    for (int i = 0; i < 3; i++) {
        t1[i] = t[i][0] / s;
        t2[i] = -t[i][0] / s;
    }


    float W[3][3] = { {0,-1,0},{1,0,0},{0,0,1} };
    float Wt[3][3] = { {0,1,0},{-1,0,0},{0,0,1} };

    matMult(*u, *W, B, 3, 3, 3);
    matMult(B, *vt, *R1, 3, 3, 3);
    s = (R1[0][0] * R1[1][1] * R1[2][2] + R1[0][1] * R1[1][2] * R1[2][0] + R1[0][2] * R1[1][0] * R1[2][1] -
        R1[0][2] * R1[1][1] * R1[2][0] - R1[0][1] * R1[1][0] * R1[2][2] - R1[0][0] * R1[1][2] * R1[2][1]);
    if (s < 0)
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                R1[i][j] = -R1[i][j];

    matMult(*u, *Wt, B, 3, 3, 3);
    matMult(B, *vt, *R2, 3, 3, 3);
    s = (R2[0][0] * R2[1][1] * R2[2][2] + R2[0][1] * R2[1][2] * R2[2][0] + R2[0][2] * R2[1][0] * R2[2][1] -
        R2[0][2] * R2[1][1] * R2[2][0] - R2[0][1] * R2[1][0] * R2[2][2] - R2[0][0] * R2[1][2] * R2[2][1]);
    if (s < 0)
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                R2[i][j] = -R2[i][j];
    int ii = 0, wh = 0 ,wt = 0;
    io = 0;
    if (ii < io || io == 0) {
        checkRT1(saveKnP, K, *R1, t1);
        ii = io;
        wh = 1;
        wt = 1;
    }
    if (ii < io || io == 0) {
        checkRT1(saveKnP, K, *R1, t2);
        ii = io;
        wh = 1;
        wt = 0;
    }
    if (ii < io || io == 0) {
        checkRT1(saveKnP, K, *R2, t1);
        wh = 0;
        wt = 1;
        ii = io;
    }
    if (ii < io || io == 0) {
        checkRT1(saveKnP, K, *R2, t2);
        wh = 0;
        wt = 0;
        ii = io;
    }

    if (wh == 1) {
        xpt = fastatan2(R1[2][1], R1[2][2]);
        ypt = fastatan2(-R1[2][0], sqrt(R1[2][1] * R1[2][1] + R1[2][2] * R1[2][2]));
        zpt = fastatan2(R1[1][0], R1[0][0]);
    }
    else {
        xpt = fastatan2(R2[2][1], R2[2][2]);
        ypt = fastatan2(-R2[2][0], sqrt(R2[2][1] * R2[2][1] + R2[2][2] * R2[2][2]));
        zpt = fastatan2(R2[1][0], R2[0][0]);
    }

    if (xpt > 180) xpt -= 360;
    if (ypt > 180) ypt -= 360;
    if (zpt > 180) zpt -= 360;

    if (wt == 1) {
        xtt = t1[0];
        ytt = t1[1];
        ztt = t1[2];
    }
    else {
        xtt = t2[0];
        ytt = t2[1];
        ztt = t2[2];
    }

    return false;
}

bool reconstructF(cv::Mat& F21, cv::Mat& K, SaveKnP* saveKnP)
{
    // Compute Essential Matrix from Fundamental Matrix
    cv::Mat E21 = K.t() * F21 * K;

    cv::Mat R1, R2, t;
    // Recover the 4 motion hypotheses
    // 虽然这个函数对t有归一化，但并没有决定单目整个SLAM过程的尺度
    // 因为CreateInitialMapMonocular函数对3D点深度会缩放，然后反过来对 t 有改变
    cv::Mat u, w, vt;
    cv::SVD::compute(E21, w, u, vt);

    // 对 t 有归一化，但是这个地方并没有决定单目整个SLAM过程的尺度
    // 因为CreateInitialMapMonocular函数对3D点深度会缩放，然后反过来对 t 有改变
    u.col(2).copyTo(t);
    t = t / cv::norm(t);

    cv::Mat W(3, 3, CV_32F, cv::Scalar(0));
    W.at<float>(0, 1) = -1;
    W.at<float>(1, 0) = 1;
    W.at<float>(2, 2) = 1;

    R1 = u * W * vt;
    if (cv::determinant(R1) < 0) // 旋转矩阵有行列式为1的约束
        R1 = -R1;

    R2 = u * W.t() * vt;
    if (cv::determinant(R2) < 0)
        R2 = -R2;

    cv::Mat t1 = t;
    cv::Mat t2 = -t;

    io = 0;
    checkRT(saveKnP, K, R1, t1);
    checkRT(saveKnP, K, R1, t2);
    checkRT(saveKnP, K, R2, t1);
    checkRT(saveKnP, K, R2, t2);

    return false;
}

void checkRT1(SaveKnP * saveKnP, float * K, float * R, float * t)
{
    float P1[3][4] = {
        {K[0],K[1],K[2],0},
        {K[3],K[4],K[5],0},
        {K[6],K[7],K[8],0}
    };

    float P2T[3][4] = {
        {R[0],R[1],R[2],t[0]},
        {R[3],R[4],R[5],t[1]},
        {R[6],R[7],R[8],t[2]}
    };

    float P2[3][4] = { 0 };
    matMult(K, *P2T, *P2, 3, 3, 4);

    float O1[3] = {0,0,0};
    // 第二个相机的光心在世界坐标系下的坐标
    float O2[3] = {
        {-R[0] * t[0] + -R[3] * t[1] + -R[6] * t[2]},
        {-R[1] * t[0] + -R[4] * t[1] + -R[7] * t[2]},
        {-R[2] * t[0] + -R[5] * t[1] + -R[8] * t[2]}
    };

    int nGood = 0;
    for (int i = 0; i < connectTimes; i++)
    {
        float A[4][4] = {
            {saveKnP[i].b_X * P1[2][0] - P1[0][0], saveKnP[i].b_X * P1[2][1] - P1[0][1], saveKnP[i].b_X * P1[2][2] - P1[0][2], saveKnP[i].b_X * P1[2][3] - P1[0][3]},
            {saveKnP[i].b_Y * P1[2][0] - P1[1][0], saveKnP[i].b_Y * P1[2][1] - P1[1][1], saveKnP[i].b_Y * P1[2][2] - P1[1][2], saveKnP[i].b_Y * P1[2][3] - P1[1][3]},
            {saveKnP[i].n_X * P2[2][0] - P2[0][0], saveKnP[i].n_X * P2[2][1] - P2[0][1], saveKnP[i].n_X * P2[2][2] - P2[0][2], saveKnP[i].n_X * P2[2][3] - P2[0][3]},
            {saveKnP[i].n_Y * P2[2][0] - P2[1][0], saveKnP[i].n_Y * P2[2][1] - P2[1][1], saveKnP[i].n_Y * P2[2][2] - P2[1][2], saveKnP[i].n_Y * P2[2][3] - P2[1][3]}
        };


        float w[4][1] = { 0 };
        float u[4][4] = { 0 };
        float vt[4][4] = { 0 };
        svd(*A, *w, *u, *vt, 4, 4);
    
        float x3D[3] = { vt[3][0] / vt[3][3],  vt[3][1] / vt[3][3],  vt[3][2] / vt[3][3]};
        
        float dist1 = 0, dist2 = 0,twoND = 0;
        for (int j = 0; j < 3; j++) {
            float normal1 = x3D[j] - O1[j];
            float normal2 = x3D[j] - O2[j];
            dist1 += normal1 * normal1;
            dist2 += normal2 * normal2;
            twoND += normal1 * normal2;
        }
        float cosParallax = twoND / (dist1 * dist2);

        if (x3D[2] <= 0)
            continue;
        float x3D2[3] = { 0 };
        matMult(R,x3D,x3D2,3,3,1);
        x3D2[0] += t[0];  
        x3D2[1] += t[1];  
        x3D2[2] += t[2];

        if (x3D2[2] <= 0 && cosParallax < 0.99998)
            continue;


        float im1x, im1y;
        float invZ1 = 1.0 / x3D[2];
        im1x = K[0] * x3D[0] * invZ1 + K[2];
        im1y = K[4] * x3D[1] * invZ1 + K[5];

        float squareError1 = (im1x - saveKnP[i].b_X) * (im1x - saveKnP[i].b_X) + (im1y - saveKnP[i].b_Y) * (im1y - saveKnP[i].b_Y);

        if (squareError1 > 4)
            continue;

        float im2x, im2y;
        float invZ2 = 1.0 / x3D2[2];
        im2x = K[0] * x3D2[0] * invZ2 + K[2];
        im2y = K[4] * x3D2[1] * invZ2 + K[5];

        float squareError2 = (im2x - saveKnP[i].n_X) * (im2x - saveKnP[i].n_Y) + (im2y - saveKnP[i].n_X) * (im2y - saveKnP[i].n_Y);

        if (squareError2 > 4)
            continue;

        nGood++;
    }

    if (io < nGood) {
        io = nGood;
    }
}

void checkRT(SaveKnP* saveKnP, const cv::Mat& K, const cv::Mat& R,const cv::Mat& t)
{
    const float fx = K.at<float>(0, 0);
    const float fy = K.at<float>(1, 1);
    const float cx = K.at<float>(0, 2);
    const float cy = K.at<float>(1, 2);
    // Camera 1 Projection Matrix K[I|0]
    // 步骤1：得到一个相机的投影矩阵
    // 以第一个相机的光心作为世界坐标系
    cv::Mat P1(3, 4, CV_32F, cv::Scalar(0));
    K.copyTo(P1.rowRange(0, 3).colRange(0, 3));
    // 第一个相机的光心在世界坐标系下的坐标
    cv::Mat O1 = cv::Mat::zeros(3, 1, CV_32F);

    // Camera 2 Projection Matrix K[R|t]
    // 步骤2：得到第二个相机的投影矩阵
    cv::Mat P2(3, 4, CV_32F);
    R.copyTo(P2.rowRange(0, 3).colRange(0, 3));
    t.copyTo(P2.rowRange(0, 3).col(3));
    P2 = K * P2;
    // 第二个相机的光心在世界坐标系下的坐标
    cv::Mat O2 = -R.t() * t;

    int nGood = 0; 
    float saveKnPS[MAX_CONNECT] = { 0 };

    for (int i = 0; i < connectTimes; i++)
    {        
        // 步骤3：利用三角法恢复三维点x3D
        cv::Mat x3D;
        cv::Mat A(4, 4, CV_32F);
        // 在DecomposeE函数和ReconstructH函数中对t有归一化
        // 这里三角化过程中恢复的3D点深度取决于 t 的尺度，
        // 但是这里恢复的3D点并没有决定单目整个SLAM过程的尺度
        // 因为CreateInitialMapMonocular函数对3D点深度会缩放，然后反过来对 t 有改变
        A.row(0) = saveKnP[i].b_X * P1.row(2) - P1.row(0);
        A.row(1) = saveKnP[i].b_Y * P1.row(2) - P1.row(1);
        A.row(2) = saveKnP[i].n_X * P2.row(2) - P2.row(0);
        A.row(3) = saveKnP[i].n_Y * P2.row(2) - P2.row(1);

        cv::Mat u, w, vt;
        cv::SVD::compute(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);
        x3D = vt.row(3).t();
        x3D = x3D.rowRange(0, 3) / x3D.at<float>(3);


        // Check parallax
        // 步骤4：计算视差角余弦值
        cv::Mat normal1 = x3D - O1;
        float dist1 = cv::norm(normal1);

        cv::Mat normal2 = x3D - O2;
        float dist2 = cv::norm(normal2);

        float cosParallax = normal1.dot(normal2) / (dist1 * dist2);

        if (x3D.at<float>(2) <= 0)
            continue;

        cv::Mat x3D2 = R * x3D + t;

        if (x3D2.at<float>(2) <= 0 && cosParallax < 0.99998)
            continue;

        // 步骤6：计算重投影误差

        // Check reprojection error in first image
        // 计算3D点在第一个图像上的投影误差
        float im1x, im1y;
        float invZ1 = 1.0 / x3D.at<float>(2);
        im1x = fx * x3D.at<float>(0) * invZ1 + cx;
        im1y = fy * x3D.at<float>(1) * invZ1 + cy;

        float squareError1 = (im1x - saveKnP[i].b_X) * (im1x - saveKnP[i].b_X) + (im1y - saveKnP[i].b_Y) * (im1y - saveKnP[i].b_Y);

        // 步骤6.1：重投影误差太大，跳过淘汰
        // 一般视差角比较小时重投影误差比较大
        if (squareError1 > 4)
            continue;

        // Check reprojection error in second image
        // 计算3D点在第二个图像上的投影误差
        float im2x, im2y;
        float invZ2 = 1.0 / x3D2.at<float>(2);
        im2x = fx * x3D2.at<float>(0) * invZ2 + cx;
        im2y = fy * x3D2.at<float>(1) * invZ2 + cy;

        float squareError2 = (im2x - saveKnP[i].n_X) * (im2x - saveKnP[i].n_Y) + (im2y - saveKnP[i].n_X) * (im2y - saveKnP[i].n_Y);

        // 步骤6.2：重投影误差太大，跳过淘汰
        // 一般视差角比较小时重投影误差比较大
        if (squareError2 > 4)
            continue;


        // 步骤7：统计经过检验的3D点个数，记录3D点视差角
        nGood++;
        saveKnPS[i] = sqrt(x3D.at<float>(0) * x3D.at<float>(0) + x3D.at<float>(1) * x3D.at<float>(1) + x3D.at<float>(2) * x3D.at<float>(2));
    }

    if (io < nGood) {
        io = nGood;
    }
}

/*
* 
*             cv::Mat Ha;
            for (int j = 0; j < 8; j++) {
                cv::Mat A(16, 9, CV_32F); // 2N*9
                for (int i = 0; i < 8; i++)
                {
                    int u1 = saveKnP[i + j][0];
                    int v1 = saveKnP[i + j][1];
                    int u2 = saveKnP[i + j][2];
                    int v2 = saveKnP[i + j][3];

                    A.at<float>(2 * i, 0) = 0.0;
                    A.at<float>(2 * i, 1) = 0.0;
                    A.at<float>(2 * i, 2) = 0.0;
                    A.at<float>(2 * i, 3) = -u1;
                    A.at<float>(2 * i, 4) = -v1;
                    A.at<float>(2 * i, 5) = -1;
                    A.at<float>(2 * i, 6) = v2 * u1;
                    A.at<float>(2 * i, 7) = v2 * v1;
                    A.at<float>(2 * i, 8) = v2;

                    A.at<float>(2 * i + 1, 0) = u1;
                    A.at<float>(2 * i + 1, 1) = v1;
                    A.at<float>(2 * i + 1, 2) = 1;
                    A.at<float>(2 * i + 1, 3) = 0.0;
                    A.at<float>(2 * i + 1, 4) = 0.0;
                    A.at<float>(2 * i + 1, 5) = 0.0;
                    A.at<float>(2 * i + 1, 6) = -u2 * u1;
                    A.at<float>(2 * i + 1, 7) = -u2 * v1;
                    A.at<float>(2 * i + 1, 8) = -u2;

                }
                cv::Mat u, w, vt;

                cv::SVDecomp(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);

                Ha = vt.row(8).reshape(0, 3); // v的最后一列

                for (int i = 0; i < 3; i++)
                    for (int k = 0; k < 3; k++)
                        Ha.at<float>(i,k) = Ha.at<float>(i, k) / Ha.at<float>(2, 2);

                loss = 0;
                for (int i = 0; i < 8; i++) {
                    int u1 = saveKnP[i + j][0];
                    int v1 = saveKnP[i + j][1];
                    int u2 = saveKnP[i + j][2];
                    int v2 = saveKnP[i + j][3];
                    float dis = (u1 * Ha.at<float>(2, 0) + v1 * Ha.at<float>(2, 1) + 1 * Ha.at<float>(2, 2));
                    loss += pow(u2 - (u1 * Ha.at<float>(0, 0) + v1 * Ha.at<float>(0, 1) + 1 * Ha.at<float>(0, 2)) / dis, 2) +
                        pow(v2 - (u1 * Ha.at<float>(1, 0) + v1 * Ha.at<float>(1, 1) + 1 * Ha.at<float>(1, 2)) / dis, 2);
                }
                if (loss < 50) break;
            }
            //for (int j = 0; j < 3; j++) {
            //    cout << Ha.at<float>(j, 0) << " , " << Ha.at<float>(j, 1) << " , " << Ha.at<float>(j, 2) << endl;
            //}
            Mat K2 = (Mat_<float>(3, 3) <<
                400*2, 0, INPUT_W ,
                0, 400*2, INPUT_H ,
                0, 0, 1);
            reconstructH(Ha, K2);
* 
*    
*                 //brief
                for (ptidx = 0; ptidx < 1; ptidx++)
                {
                    float angle = S_keypoint.angle;
                    angle *= (float)(3.141592 / 180.f);
                    float a = (float)cos(angle), b = (float)sin(angle);

                    //if (angle <= 180)
                    //    angle = -angle + 180;
                    //else
                    //    angle = (-angle + 180) + 360;
                    //angle /= 360;
                    const unsigned char* center = (*input[2]) + S_keypoint.axis[1] * INPUT_W + S_keypoint.axis[0];
                    float x, y;
                    int ix, iy;
                    Pattern* pattern = &_pattern[0];
                    unsigned char* desc = S_keypoint.descriptors;
                    //#if 1    
                    //#define     GET_VALUE(idx) \
                        //       (rp = bit_pattern_list[idx],\
                        //        rs = idx + int(angle * k_list[rp]),\
                        //        ix = k_size[rp][rs][0], \
                        ////        iy = k_size[rp][rs][1], \
                        //        *(center + iy*INPUT_W + ix) )
                        //#endif
#if 1  
#define GET_VALUE(idx) \
               (x = (pattern[idx].x*a - pattern[idx].y*b), \
                y = (pattern[idx].x*b + pattern[idx].y*a), \
                ix = (int)(x + (x >= 0 ? 0.5f : -0.5f)), \
                iy = (int)(y + (y >= 0 ? 0.5f : -0.5f)), \
                *(center + iy*INPUT_W + ix) )
#endif
                    for (int i = 0; i < 16; ++i, pattern += 16)//每個特徵描述子長度爲8個字節  
                    {
                        int t0, t1, val;
                        t0 = GET_VALUE(0); t1 = GET_VALUE(1);
                        val = (t0 + 2 < t1);
                        t0 = GET_VALUE(2); t1 = GET_VALUE(3);
                        val |= (t0 + 2 < t1) << 1;
                        t0 = GET_VALUE(4); t1 = GET_VALUE(5);
                        val |= (t0 + 2 < t1) << 2;
                        t0 = GET_VALUE(6); t1 = GET_VALUE(7);
                        val |= (t0 + 2 < t1) << 3;
                        t0 = GET_VALUE(8); t1 = GET_VALUE(9);
                        val |= (t0 + 2 < t1) << 4;
                        t0 = GET_VALUE(10); t1 = GET_VALUE(11);
                        val |= (t0 + 2 < t1) << 5;
                        t0 = GET_VALUE(12); t1 = GET_VALUE(13);
                        val |= (t0 + 2 < t1) << 6;
                        t0 = GET_VALUE(14); t1 = GET_VALUE(15);
                        val |= (t0 + 2 < t1) << 7;

                        desc[i] = (unsigned char)val;
                    }
#undef GET_VALUE
                }

    Mat K2 = (Mat_<float>(3, 3) << 1, 0, 240, 0, 1, 135, 0, 0, 1);
    Mat F2 = (Mat_<float>(3, 3) << F[0][0], F[0][1], F[0][2], F[1][0], F[1][1], F[1][2], F[2][0], F[2][1], F[2][2]);


    cv::Mat A(8, 9, CV_32F);
    for (int i = 0; i < 8; i++)
    {
        const float u1 = keypoint[0][saveKnP[i][1]].axis[0];
        const float v1 = keypoint[0][saveKnP[i][1]].axis[1];
        const float u2 = keypoint[1][saveKnP[i][0]].axis[0];
        const float v2 = keypoint[1][saveKnP[i][0]].axis[1];

        A.at<float>(i, 0) = u2 * u1;
        A.at<float>(i, 1) = u2 * v1;
        A.at<float>(i, 2) = u2;
        A.at<float>(i, 3) = v2 * u1;
        A.at<float>(i, 4) = v2 * v1;
        A.at<float>(i, 5) = v2;
        A.at<float>(i, 6) = u1;
        A.at<float>(i, 7) = v1;
        A.at<float>(i, 8) = 1;
    }
    cv::Mat u, w, vt;
    cv::SVDecomp(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);
    cv::Mat Fpre = vt.row(8).reshape(0, 3); // v的最后一列

    cv::SVDecomp(Fpre, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);
    w.at<float>(2) = 0; // 秩2约束，将第3个奇异值设为0
    cv::Mat C = u * cv::Mat::diag(w) * vt;
    reconstructF(F2, K2);

Mat A = (Mat_<float>(16, 9) <<
    100, 100, 1, 0, 0, 0, -10200, -10200, -102,
    0, 0, 0, -100, -100, -1, 17900, 17900, 179,
    150, 180, 1, 0, 0, 0, -28200, -33840, -188,
    0, 0, 0, -150, -180, -1, 33150, 39780, 221,
    250, 150, 1, 0, 0, 0, -64500, -38700, -258,
    0, 0, 0, -250, -150, -1, 35500, 21300, 142,
    158, 190, 1, 0, 0, 0, -31758, -38190, -201,
    0, 0, 0, -158, -190, -1, 35708, 42940, 226,
    240, 135, 1, 0, 0, 0, -57840, -32535, -241,
    0, 0, 0, -240, -135, -1, 32400, 18225, 135,
    80, 35, 1, 0, 0, 0, -4000, -1750, -50,
    0, 0, 0, -80, -35, -1, 10720, 4690, 134,
    480, 270, 1, 0, 0, 0, -249120, -140130, -519,
    0, 0, 0, -480, -270, -1, 59520, 33480, 124,
    330, 140, 1, 0, 0, 0, -105930, -44940, -321,
    0, 0, 0, -330, -140, -1, 30030, 12740, 91
);

    cv::Mat A(2 * 8, 9, CV_32F); // 2N*9

    for (int i = 0; i < 9; i++)
    {
        int u1 = keypoint[0][saveKnP[i][1]].axis[0];
        int v1 = keypoint[0][saveKnP[i][1]].axis[1];
        int u2 = keypoint[1][saveKnP[i][0]].axis[0];
        int v2 = keypoint[1][saveKnP[i][0]].axis[1];

        A.at<float>(2 * i, 0) = 0.0;
        A.at<float>(2 * i, 1) = 0.0;
        A.at<float>(2 * i, 2) = 0.0;
        A.at<float>(2 * i, 3) = -u1;
        A.at<float>(2 * i, 4) = -v1;
        A.at<float>(2 * i, 5) = -1;
        A.at<float>(2 * i, 6) = v2 * u1;
        A.at<float>(2 * i, 7) = v2 * v1;
        A.at<float>(2 * i, 8) = v2;

        A.at<float>(2 * i + 1, 0) = u1;
        A.at<float>(2 * i + 1, 1) = v1;
        A.at<float>(2 * i + 1, 2) = 1;
        A.at<float>(2 * i + 1, 3) = 0.0;
        A.at<float>(2 * i + 1, 4) = 0.0;
        A.at<float>(2 * i + 1, 5) = 0.0;
        A.at<float>(2 * i + 1, 6) = -u2 * u1;
        A.at<float>(2 * i + 1, 7) = -u2 * v1;
        A.at<float>(2 * i + 1, 8) = -u2;

    }
    cv::Mat u, w, vt;
    cv::SVDecomp(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);


Mat w, u, vt;
SVDecomp(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);
for (int i = 0; i < 9; i++) {
    cout << vt.row(8).reshape(0, 3)/vt.at<float>(8,8) << endl;
}

vector<vector<float>> B{
{100, 100, 1, 0, 0, 0, -10200, -10200, -102},
{ 0 ,0 ,0 ,-100 ,-100 ,-1 ,17900 ,17900 ,179},
{ 150 ,180 ,1 ,0 ,0 ,0 ,-28200 ,-33840 ,-188},
{ 0 ,0 ,0 ,-150 ,-180 ,-1 ,33150 ,39780 ,221},
{ 250 ,150 ,1 ,0 ,0 ,0 ,-64500 ,-38700 ,-258},
{ 0 ,0 ,0 ,-250 ,-150 ,-1 ,35500 ,21300 ,142},
{ 158 ,190 ,1 ,0 ,0 ,0 ,-31758 ,-38190 ,-201},
{ 0 ,0 ,0 ,-158 ,-190 ,-1 ,35708 ,42940 ,226},
{ 240 ,135 ,1 ,0 ,0 ,0 ,-57840 ,-32535 ,-241},
{ 0 ,0 ,0 ,-240 ,-135 ,-1 ,32400 ,18225 ,135},
{ 80 ,35 ,1 ,0 ,0 ,0 ,-4000 ,-1750 ,-50},
{ 0 ,0 ,0 ,-80 ,-35 ,-1 ,10720 ,4690 ,134},
{ 480 ,270 ,1 ,0 ,0 ,0 ,-249120 ,-140130 ,-519},
{ 0 ,0 ,0 ,-480 ,-270 ,-1 ,59520 ,33480 ,124},
{ 330 ,140 ,1 ,0 ,0 ,0 ,-105930 ,-44940 ,-321},
{ 0 ,0 ,0 ,-330 ,-140 ,-1 ,30030 ,12740 ,91}
    };

    vector<vector<float>> w2, u2, vt2;
    START = getTickCount();
    svd1(B, w2, u2, vt2);

    for (int i = 0; i < 9; i++) {
        cout << vt2[8][i] / vt2[8][8] << " , ";
    }

void JacobiSVD1(std::vector<std::vector<float>>& At, std::vector<std::vector<float>>& _W, std::vector<std::vector<float>>& Vt)
{
    double minval = FLT_MIN;
    auto eps = (float)(FLT_EPSILON * 2);
    const int m = At[0].size();
    const auto n = (int)_W.size();
    const int n1 = m; // urows
    std::vector<double> W(_W.size(), 0.);

    for (int i = 0; i < n; i++) {
        double sd{ 0. };
        for (int k = 0; k < m; k++) {
            float t = At[i][k];
            sd += (double)t * t;
        }
        W[i] = sd;

        for (int k = 0; k < n; k++)
            Vt[i][k] = 0;
        Vt[i][i] = 1;
    }

    int max_iter = std::max(m, 30);
    for (int iter = 0; iter < max_iter; iter++) {
        bool changed = false;
        float c, s;

        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                float* Ai = At[i].data(), * Aj = At[j].data();
                double a = W[i], p = 0, b = W[j];

                for (int k = 0; k < m; k++)
                    p += (double)Ai[k] * Aj[k];

                if (std::abs(p) <= eps * std::sqrt((double)a * b))
                    continue;

                p *= 2;
                double beta = a - b, gamma = hypot((double)p, beta);
                if (beta < 0) {
                    double delta = (gamma - beta) * 0.5;
                    s = (float)std::sqrt(delta / gamma);
                    c = (float)(p / (gamma * s * 2));
                }
                else {
                    c = (float)std::sqrt((gamma + beta) / (gamma * 2));
                    s = (float)(p / (gamma * c * 2));
                }

                a = b = 0;
                for (int k = 0; k < m; k++) {
                    float t0 = c * Ai[k] + s * Aj[k];
                    float t1 = -s * Ai[k] + c * Aj[k];
                    Ai[k] = t0; Aj[k] = t1;

                    a += (double)t0 * t0; b += (double)t1 * t1;
                }
                W[i] = a; W[j] = b;

                changed = true;

                float* Vi = Vt[i].data(), * Vj = Vt[j].data();

                for (int k = 0; k < n; k++) {
                    float t0 = c * Vi[k] + s * Vj[k];
                    float t1 = -s * Vi[k] + c * Vj[k];
                    Vi[k] = t0; Vj[k] = t1;
                }
            }
        }

        if (!changed)
            break;
    }

    for (int i = 0; i < n; i++) {
        double sd{ 0. };
        for (int k = 0; k < m; k++) {
            float t = At[i][k];
            sd += (double)t * t;
        }
        W[i] = std::sqrt(sd);
    }

    for (int i = 0; i < n - 1; i++) {
        int j = i;
        for (int k = i + 1; k < n; k++) {
            if (W[j] < W[k])
                j = k;
        }
        if (i != j) {
            std::swap(W[i], W[j]);

            for (int k = 0; k < m; k++)
                std::swap(At[i][k], At[j][k]);

            for (int k = 0; k < n; k++)
                std::swap(Vt[i][k], Vt[j][k]);
        }
    }

    for (int i = 0; i < n; i++)
        _W[i][0] = (float)W[i];

    srand(time(nullptr));

    for (int i = 0; i < n1; i++) {
        double sd = i < n ? W[i] : 0;

        for (int ii = 0; ii < 100 && sd <= minval; ii++) {
            // if we got a zero singular value, then in order to get the corresponding left singular vector
            // we generate a random vector, project it to the previously computed left singular vectors,
            // subtract the projection and normalize the difference.
            const auto val0 = (float)(1. / m);
            for (int k = 0; k < m; k++) {
                unsigned long int rng = rand() % 4294967295; // 2^32 - 1
                float val = (rng & 256) != 0 ? val0 : -val0;
                At[i][k] = val;
            }
            for (int iter = 0; iter < 2; iter++) {
                for (int j = 0; j < i; j++) {
                    sd = 0;
                    for (int k = 0; k < m; k++)
                        sd += At[i][k] * At[j][k];
                    float asum = 0;
                    for (int k = 0; k < m; k++) {
                        auto t = (float)(At[i][k] - sd * At[j][k]);
                        At[i][k] = t;
                        asum += std::abs(t);
                    }
                    asum = asum > eps * 100 ? 1 / asum : 0;
                    for (int k = 0; k < m; k++)
                        At[i][k] *= asum;
                }
            }

            sd = 0;
            for (int k = 0; k < m; k++) {
                float t = At[i][k];
                sd += (double)t * t;
            }
            sd = std::sqrt(sd);
        }

        float s = (float)(sd > minval ? 1 / sd : 0.);
        for (int k = 0; k < m; k++)
            At[i][k] *= s;
    }
}

int svd1(const std::vector<std::vector<float>>& matSrc, std::vector<std::vector<float>>& matD, std::vector<std::vector<float>>& matU, std::vector<std::vector<float>>& matVt)
{
    int m = matSrc.size();
    int n = matSrc[0].size();

    for (const auto& sz : matSrc) {
        if (n != sz.size()) {
            fprintf(stderr, "matrix dimension dismatch\n");
            return -1;
        }
    }

    bool at = false;
    if (m < n) {
        std::swap(m, n);
        at = true;
    }

    matD.resize(n);
    for (int i = 0; i < n; ++i) {
        matD[i].resize(1, (float)0);
    }
    matU.resize(m);
    for (int i = 0; i < m; ++i) {
        matU[i].resize(m, (float)0);
    }
    matVt.resize(n);
    for (int i = 0; i < n; ++i) {
        matVt[i].resize(n, (float)0);
    }
    std::vector<std::vector<float>> tmp_u = matU, tmp_v = matVt;
    std::vector<std::vector<float>> tmp_a(n, vector<float>());
    std::vector<std::vector<float>> tmp_a_;

    if (!at)
    {
        //transpose(matSrc, tmp_a);
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                tmp_a[j].push_back(matSrc[i][j]);
            }
        }
    }
    else
        tmp_a = matSrc;

    if (m == n) {
        tmp_a_ = tmp_a;
    }
    else {
        tmp_a_.resize(m);
        for (int i = 0; i < m; ++i) {
            tmp_a_[i].resize(m, (float)0);
        }
        for (int i = 0; i < n; ++i) {
            tmp_a_[i].assign(tmp_a[i].begin(), tmp_a[i].end());
        }
    }

    JacobiSVD1(tmp_a_, matD, tmp_v);

    if (!at) {
        matVt = tmp_v;
        //transpose(tmp_a_, matU);
    }
    else {
        //        transpose(tmp_v, matVt);
        //        matU = tmp_a_;
    }

    return 0;
}
    for (int h = STP; h < INPUT_H - STP; h++) {
        unsigned char* ptr = input[pp][h] + STP;
        unsigned char* curr = buf[(h - STP) % 3];
        float* ang_curr = ang_buf[(h - STP) % 3];
        int ncorners = 0;
        int* cornerpos = cpbuf[(h - STP) % 3];

        for (int i = 0; i < INPUT_W; i++) {
            *(curr + i) = 0;
        }

        for (int w = STP; w < INPUT_W - STP; w++, ptr++) {
            int v = ptr[0];
            const unsigned char* tab = &threshold_tab[0] - v + 255;
            //cout << int(ptr[pixel[8]]) << endl;
            //cout << tab[ptr[pixel[8]]] << endl;
            int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];

            if (d == 0)
                continue;

            d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
            d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
            d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];
            if (d == 0)
                continue;

            d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
            d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
            d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
            d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];


            if (d & 1) {
                int vt = v - 10, count = 0;
                for (int k = 0; k < 25; k++)
                {
                    int x = ptr[pixel[k]];
                    if (x < vt)
                    {
                        if (++count > 8)
                        {
                            cornerpos[ncorners++] = w;
                            curr[w] = (unsigned char)cornerScore(ptr, pixel, 10);
                            // ang_curr[w] = 360-360/16 * (k % 16);
                            break;
                        }
                    }
                    else
                        count = 0;
                }
            }

            if (d & 2) {
                int vt = v + 10, count = 0;
                for (int k = 0; k < 25; k++)
                {
                    int x = ptr[pixel[k]];
                    if (x > vt)
                    {
                        if (++count > 8)
                        {
                            cornerpos[ncorners++] = w;
                            curr[w] = (unsigned char)cornerScore(ptr, pixel, 10);
                            //ang_curr[w] = 360- 360 / 16 * (k % 16);
                            break;
                        }
                    }
                    else
                        count = 0;
                }
            }

        }
        cornerpos[-1] = ncorners;
        if (h == STP)
            continue;

        const unsigned char* prev = buf[(h - STP + 2) % 3];
        const unsigned char* pprev = buf[(h - STP + 1) % 3];
        ncorners = cornerpos[-1];

        for (int k = 0; k < ncorners; k++) {
            int w = cornerpos[k];
            int score = prev[w];
            if ((score > prev[w + 1] && score > prev[w - 1] &&
                score > pprev[w - 1] && score > pprev[w] && score > pprev[w + 1] &&
                score > curr[w - 1] && score > curr[w] && score > curr[w + 1]))
            {
                int sh = h * SLICE_TIMES / INPUT_H;
                int sw = w * SLICE_TIMES / INPUT_W;
                if (keypointO[pp][sh][sw] == 0) {
                    keypoint[pp][oo].axis[0] = w;
                    keypoint[pp][oo].axis[1] = h;
                    keypointO[pp][sh][sw] = oo;
                    keypoint[pp][oo].response = (float)score;
                    keypoint[pp][oo].angle = ang_curr[w];
                    oo += 1;
                }
                else if (keypoint[pp][keypointO[pp][sh][sw]].response < score) {
                    keypoint[pp][keypointO[pp][sh][sw]].axis[0] = w;
                    keypoint[pp][keypointO[pp][sh][sw]].axis[1] = h;
                    keypoint[pp][keypointO[pp][sh][sw]].response = (float)score;
                    keypoint[pp][keypointO[pp][sh][sw]].angle = ang_curr[w];
                }
            }
        }
    }

    int ptidx = 0;
    oo = 0;

    const unsigned char* ptr00 = *input[pp];
    int r = BLOCK_SIZE / 2;
    //harris 角點分數
    for (ptidx = 0; ptidx < MAX_ST; ptidx++)
    {
        int x0 = keypoint[pp][ptidx].axis[0];
        if (x0 == 0)
            break;

        int y0 = keypoint[pp][ptidx].axis[1];
        const unsigned char* ptr0 = ptr00 + (y0 - r) * INPUT_W + (x0 - r);  //目標點向上r格再向左r格的像素點
        int a = 0, b = 0, c = 0;
        for (int k = 0; k < BLOCK_SIZE * BLOCK_SIZE; k++)
        {
            const unsigned char* ptr = ptr0 + ofs[k];
            int Ix = (ptr[1] - ptr[-1]) * 2 + (ptr[-INPUT_W + 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[INPUT_W - 1]);
            int Iy = (ptr[INPUT_W] - ptr[-INPUT_W]) * 2 + (ptr[INPUT_W - 1] - ptr[-INPUT_W - 1]) + (ptr[INPUT_W + 1] - ptr[-INPUT_W + 1]);
            a += Ix * Ix;
            b += Iy * Iy;
            c += Ix * Iy;
        }
        keypoint[pp][ptidx].response = ((float)a * b - (float)c * c - HARRIS_K * ((float)a + b) * ((float)a + b)) * scale_sq_sq;
    }

    //IC_Angle
    for (ptidx = 0; ptidx < MAX_ST; ptidx++)
    {
        int x0 = keypoint[pp][ptidx].axis[0];
        if (x0 == 0)
            break;
        const unsigned char* center = (*input[pp]) + keypoint[pp][ptidx].axis[1] * INPUT_W + keypoint[pp][ptidx].axis[0];

        int m_01 = 0, m_10 = 0;

        for (int u = -15; u <= 15; ++u)
            m_10 += u * center[u];

        for (int v = 1; v <= 15; ++v)
        {
            int v_sum = 0;
            int d = umax[v];
            for (int u = -d; u <= d; ++u)
            {
                int val_plus = center[u + v * INPUT_W], val_minus = center[u - v * INPUT_W];
                v_sum += (val_plus - val_minus);
                m_10 += u * (val_plus + val_minus);
            }
            m_01 += v * v_sum;
        }
        keypoint[pp][ptidx].angle = fastatan2((float)m_01, (float)m_10);
    }

    gaussianBlur1(input[pp], *input[2], INPUT_H, INPUT_W);
    gaussianBlur2(input[2], *input[pp], INPUT_H, INPUT_W);

    //brief
    for (ptidx = 0; ptidx < MAX_ST; ptidx++)
    {
        int x0 = keypoint[pp][ptidx].axis[0];
        if (x0 == 0)
            break;
        float angle = keypoint[pp][ptidx].angle;
        angle *= (float)(3.141592 / 180.f);
        float a = (float)cos(angle), b = (float)sin(angle);

        //if (angle <= 180)
        //    angle = -angle + 180;
        //else
        //    angle = (-angle + 180) + 360;
        //angle /= 360;
        const unsigned char* center = (*input[pp]) + keypoint[pp][ptidx].axis[1] * INPUT_W + keypoint[pp][ptidx].axis[0];
        float x, y;
        int ix, iy;
        Pattern* pattern = &_pattern[0];
        unsigned char* desc = keypoint[pp][ptidx].descriptors;
        //#if 1
        //#define     GET_VALUE(idx) \
            //       (rp = bit_pattern_list[idx],\
            //        rs = idx + int(angle * k_list[rp]),\
            //        ix = k_size[rp][rs][0], \
            ////        iy = k_size[rp][rs][1], \
            //        *(center + iy*INPUT_W + ix) )
            //#endif
#if 1
#define GET_VALUE(idx) \
               (x = (pattern[idx].x*a - pattern[idx].y*b), \
                y = (pattern[idx].x*b + pattern[idx].y*a), \
                ix = (int)(x + (x >= 0 ? 0.5f : -0.5f)), \
                iy = (int)(y + (y >= 0 ? 0.5f : -0.5f)), \
                *(center + iy*INPUT_W + ix) )
#endif
        for (int i = 0; i < 16; ++i, pattern += 16)//每個特徵描述子長度爲8個字節
        {
            int t0, t1, val;
            t0 = GET_VALUE(0); t1 = GET_VALUE(1);
            val = (t0 + 2 < t1);
            t0 = GET_VALUE(2); t1 = GET_VALUE(3);
            val |= (t0 + 2 < t1) << 1;
            t0 = GET_VALUE(4); t1 = GET_VALUE(5);
            val |= (t0 + 2 < t1) << 2;
            t0 = GET_VALUE(6); t1 = GET_VALUE(7);
            val |= (t0 + 2 < t1) << 3;
            t0 = GET_VALUE(8); t1 = GET_VALUE(9);
            val |= (t0 + 2 < t1) << 4;
            t0 = GET_VALUE(10); t1 = GET_VALUE(11);
            val |= (t0 + 2 < t1) << 5;
            t0 = GET_VALUE(12); t1 = GET_VALUE(13);
            val |= (t0 + 2 < t1) << 6;
            t0 = GET_VALUE(14); t1 = GET_VALUE(15);
            val |= (t0 + 2 < t1) << 7;

            desc[i] = (unsigned char)val;
        }
#undef GET_VALUE
    }
    Keypoint_Sort(keypoint[pp]);


    */