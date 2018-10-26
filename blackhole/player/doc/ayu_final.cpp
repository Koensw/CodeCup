/*
 (c) 2015 Koen Wolters
 
 Final player for CodeCup Ayu competition
 
 compile options: -std=c++11 -O2
 */
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <queue>
#include <utility>
#include <climits>
#include <stack>
#include <vector>
#include <fstream>
#include <climits>
#include <cassert>
#include <sys/time.h>

#include "uint128.h"

#ifndef DEBUG
   #define DEBUG
#endif

#define MIN -1000000
#define MAX 1000000

enum PLAYER{
   ME = 0,
   HIM
};
enum HASHTYPE{
   EXACT = 0,
   UPPERBOUND,
   LOWERBOUND,
   UNKNOWN, 
   NOTFOUND
};

struct map_t{
   uint128_t brd[2];
   uint128_t grps[2][30];
   unsigned int grep[2];
   unsigned int hash;

   uint128_t mvs[2];
   int cst[2];
   int max[2];
   int cap[2];
};
struct pvs_t{
   pvs_t(): mv(0), nxt(0) {}
   uint128_t mv;
   pvs_t *nxt;
};
struct hash_t{
   uint128_t brd[2];
   
   long long score;
   uint128_t bm;
   
   HASHTYPE type;
   char ldpt;
   char dpt;
   unsigned short mcnt; //move where this hash was stored
};

map_t map;
int lastdepth = 0;
unsigned int move_cnt = 1;
double elapsed_time = 0;

#define TT_SIZE 100000
hash_t tt_tab1[TT_SIZE];
hash_t tt_tab2[TT_SIZE];

/** BIT FUNCTIONS */
//generate the border around a bitboard
inline uint128_t border(uint128_t i){  
   uint128_t l(18446673670588850171ull, 18410697675910412286ull);
   uint128_t t(18302699254377873407ull, 18446744073709551615ull);
   uint128_t r(18374651278111272957ull, 18428720874809981951ull);
   uint128_t b(18446744073709551615ull, 18446744073709549568ull);
   return (~i & (((i & r) << 1u) | ((i & l) >> 1u) | ((i & t) << 11u) | ((i & b) >> 11u)));
}
//find all corners of a group that has only one other bit connected to it
//in these way most of the valid start points can be found and all other start points are not used
inline uint128_t corners(uint128_t i){
   uint128_t sd[4];
   sd[0] = uint128_t(18446673670588850171ull, 18410697675910412286ull);
   sd[0] = (i & sd[0]) >> 1u;
   sd[1] = uint128_t(18302699254377873407ull, 18446744073709551615ull);
   sd[1] = (i & sd[1]) << 11u;
   sd[2] = uint128_t(18374651278111272957ull, 18428720874809981951ull);
   sd[2] = (i & sd[2]) << 1u;
   sd[3] = uint128_t(18446744073709551615ull, 18446744073709549568ull);
   sd[3] = (i & sd[3]) >> 11u;
   uint128_t ret;
   for(int j=0; j<4; ++j){
      uint128_t tmp = 0;
      for(int k=0; k<4; ++k){
         if(j == k) continue;
         tmp |= sd[k];
      }
      ret |= i&~tmp;
   }
   return ret;
}
inline uint128_t lsb(uint128_t i){
   return (i & (~i+1));
}
inline unsigned int lsb(unsigned int i){
   return (i & (~i+1));
}
inline unsigned int ctz(uint128_t i){
   if(i.lower()) return __builtin_ctzll(i.lower());
   else return __builtin_ctzll(i.upper())+64;
}
inline unsigned int ctz(unsigned int i){
   return __builtin_ctz(i);
}
inline int popcnt(uint128_t i){
   return __builtin_popcountll(i.lower())+__builtin_popcountll(i.upper());
}

/** TRANSPOSITION TABLE **/
//retrieve position from hash table (a pretty standard depth and non-depth hashtable are used together)
inline HASHTYPE tt_retrieve(hash_t &r, map_t &m, int dpt){  
   unsigned int hash = m.hash % TT_SIZE;
   if(tt_tab2[hash].brd[0] == m.brd[0] && tt_tab2[hash].brd[1] == m.brd[1]){
      r = tt_tab2[hash];
      if(tt_tab2[hash].mcnt == move_cnt && tt_tab2[hash].ldpt == lastdepth && dpt <= tt_tab2[hash].dpt) return tt_tab2[hash].type;
      else return UNKNOWN;
   }else if(tt_tab1[hash].brd[0] == m.brd[0] && tt_tab1[hash].brd[1] == m.brd[1]){
      r = tt_tab1[hash];
      if(tt_tab1[hash].mcnt == move_cnt && tt_tab1[hash].ldpt == lastdepth && dpt <= tt_tab1[hash].dpt) return tt_tab1[hash].type;
      else return UNKNOWN;
   }else return NOTFOUND;
}

//save position in hash table
inline void tt_store(map_t &m, long long score, uint128_t bm, HASHTYPE type, int dpt){
   unsigned int hash = m.hash % TT_SIZE;
   if(tt_tab2[hash].mcnt != move_cnt || tt_tab2[hash].ldpt != lastdepth || tt_tab2[hash].dpt <= dpt){
      tt_tab2[hash].brd[0] = m.brd[0]; tt_tab2[hash].brd[1] = m.brd[1];
      tt_tab2[hash].score = score;
      tt_tab2[hash].bm = bm;
      tt_tab2[hash].type = type;
      tt_tab2[hash].dpt = dpt;
      tt_tab2[hash].ldpt = lastdepth;
      tt_tab2[hash].mcnt = move_cnt;
   }else{
      tt_tab1[hash].brd[0] = m.brd[0]; tt_tab1[hash].brd[1] = m.brd[1];
      tt_tab1[hash].score = score;
      tt_tab1[hash].bm = bm;
      tt_tab1[hash].type = type;
      tt_tab1[hash].dpt = dpt;
      tt_tab1[hash].ldpt = lastdepth;
      tt_tab1[hash].mcnt = move_cnt;
   }
}

/** MAP **/
//initialize the map
unsigned int mp_zobrist[250];
inline map_t mp_init(PLAYER p){
   int cnt[2];
   cnt[0] = 0; cnt[1] = 0;
   bool d = false;
   map_t mp;
   mp.brd[0] = mp.brd[1] = 0;
   mp.hash = 0;
   for(int i=0; i<11; ++i){
      for(int j=0; j<11; ++j){
         if(d){
            uint128_t grp = uint128_1 << (i*11+j);
            mp.hash ^= mp_zobrist[(i*11+j)*2+p];
            mp.brd[p] |= grp;
            mp.grps[p][cnt[p]++] = grp; 
         }
         d = !d;
      }
      p = static_cast<PLAYER>(!p);
   }
   mp.grep[0]= mp.grep[1] = (1u << 30)-1;
   return mp;
}

//move a piece and update the groups information
void mp_apply(map_t &mp, uint128_t mv){
   PLAYER pl = static_cast<PLAYER>((mp.brd[0] & mv) == false);

   //find the group
   int j = -1;
   unsigned int tgrep = mp.grep[pl];
   while(tgrep){
      int i = ctz(tgrep);
      tgrep ^= lsb(tgrep);
      if(mp.grps[pl][i] & mv) {
         j = i;
         break;
      }
   }  

   //update hash
   uint128_t fr = mv & mp.brd[pl];
   mp.hash ^= mp_zobrist[2*ctz(fr)+pl];
   mp.hash ^= mp_zobrist[2*ctz(fr^mv)+pl];
   
   //update board
   mp.brd[0] ^= (uint128_1 << 127); //127 bit -> determines which side can play
   mp.brd[pl] ^= mv;
   mp.grps[pl][j] ^= mv;

   //merge groups if necessary
   uint128_t ht = border(mp.grps[pl][j]);
   tgrep = mp.grep[pl];
   while(tgrep){
      int i = ctz(tgrep);
      tgrep ^= lsb(tgrep);
      if(i == j) continue;
      if(mp.grps[pl][i] & ht) {
         //merge group
         mp.grps[pl][j] |= mp.grps[pl][i];
         mp.grep[pl] ^= (1u << i);
         mp.grps[pl][i] = 0;
      }
   }
} 

//utility function to print the map
void mp_print(map_t &mp){
   std::cerr << "   ";
   for(int i=0; i<11; ++i){
      std::cerr << static_cast<char>('A'+i);
   }
   std::cerr << std::endl;
   for(int i=0; i<11; ++i){
      std::cerr << 11-i;
      if(11-i < 10) std::cerr << " ";
      std::cerr << " ";
      for(int j=0; j<11; ++j){
         if(mp.brd[ME] & (uint128_1 << ((10-i)*11+j))) std::cerr << 'M';
         else if(mp.brd[HIM] & (uint128_1 << ((10-i)*11+j))) std::cerr << 'H';
         else std::cerr << '.';
      }
      std::cerr << std::endl;
   }
   std::cerr << std::endl;
}

//core function that generates the available moves
//moves are generated by a bfs starting from the groups 
//simultaneously with all the possible moves also the minimum spanning tree length is found (which is the most important heuristic in the program)
inline void mp_reload(map_t &mp){
   mp.cst[0] = 0; mp.cst[1] = 0;
   mp.max[0] = 0; mp.max[1] = 0;
   mp.cap[0] = 0; mp.cap[1] = 0;
   uint128_t grps[30];
   unsigned int shrt[30];

   for(int pl=0; pl<2; ++pl){
      unsigned int grep = mp.grep[pl];
      unsigned int htf = grep;
      unsigned int tgrep = grep;
      //init copied groups
      while(tgrep){
         int j = ctz(tgrep);
         grps[j] = mp.grps[pl][j];
         tgrep ^= lsb(tgrep);
      }
      
      uint128_t mrgs = 0;
      uint128_t obrd = ~mp.brd[!pl];
      uint128_t brd = mp.brd[pl];
      unsigned int cst = 0;
      unsigned int max = 0;
      int j = 0;
      
      //loop over distance from starting point
      for(j=0; grep && grep^lsb(grep); ++j){
         shrt[j] = 0;
         uint128_t sbrd = brd;
         unsigned int tgrep = grep;
         //move through groups
         while(tgrep){
            unsigned int i = ctz(tgrep);
            uint128_t ng = border(grps[i]) & obrd;
            uint128_t ht = brd & ng;
            if(!ng){
               grep ^= lsb(tgrep);
               tgrep ^= lsb(tgrep);
               ++mp.cap[pl]; 
               continue;
            }else if(ht){
               //merge point is found
               if(htf & (1u << i)){
                  if(sbrd & ng){
                     mrgs |= (border(sbrd & ng) & grps[i]);
                     shrt[j-1] |= (1u << i);
                  }else{
                     mrgs |= ht;
                     shrt[j] |= (1u << i);
                  }
                  htf ^= (1u << i);
               }
               //find matching group
               unsigned int tfgrep = grep & (~tgrep);
               while(tfgrep){
                  unsigned int k = ctz(tfgrep);
                  if(grps[k] & ht) {
                     //update the minimum spanning tree distance and maximum distance in this graph
                     unsigned int tcst = !(sbrd & grps[k] & ng) + 2*j;
                     cst += tcst;
                     max = std::max(max, tcst);
                     
                     grps[i] |= grps[k];
                     if(htf & (1u << k)) shrt[j] |= (1u << k);
                     grep ^= lsb(tfgrep);
                  }
                  tfgrep ^= lsb(tfgrep);
               }
            }
            grps[i] |= ng;
            brd |= ng;
            
            tgrep ^= lsb(tgrep);
         }
      }  
      mp.cst[pl] = cst;
      mp.max[pl] = max;
      
      //convert all merge points to moves
      uint128_t mvs = 0;
      for(int k=0; k<j; ++k){
         unsigned int tgrep = shrt[k];
         while(tgrep){
            unsigned int j = ctz(tgrep);
            tgrep ^= lsb(tgrep);
            mvs |= border(mp.grps[pl][j]) & mrgs;
         }
         mrgs |= border(mrgs) & obrd;
      }
      mp.mvs[pl] = mvs;
   }
}

/* UTILITY */
//utility function to convert 128 bit integer moves to string
std::string convert(uint128_t mv){
   unsigned int k = ctz(mv);
   unsigned int i = k/11;
   unsigned int j = k%11;
   std::string s = "";
   s += static_cast<char>('A'+j);
   if(i >= 9){
      s+='1';
      i-=10;
   }
   s += static_cast<char>('1'+i);
   return s;
}
//utility function to convert string to 128 bit integer move
uint128_t convert(std::string m){
   if(m.size() == 3) return uint128_1 << (11*(10+m[2]-'1')+m[0]-'A');
   else return uint128_1 << (11*(m[1]-'1')+m[0]-'A');
}
//utility function to apply a move in the string format
void mp_apply(map_t &mp, std::string m){
   int n = m.find('-');
   mp_apply(mp, convert(m.substr(0, n)) | convert(m.substr(n+1)));
}

/** MINIMAX **/
//position score of a board
//the board consist of six rectangles around the mid square
//the amount of stones in a rectangle is counted and weighted
//the final border has a huge negative score because it is usually bad
//further inside is scored better
inline long long pos_score(map_t &m){
   uint128_t rws[6] = {uint128_t(144080055268552710ull, 54069596698710015ull), uint128_t(35124280313865ull, 81104395049103360ull),
      uint128_t(8524956688ull, 9372063428815880192ull), uint128_t(2032160ull, 4902311062184919040ull), 
      uint128_t(448ull, 2884274086354092032ull), uint128_t(0, 1152921504606846976ull)};
   int scr[6] = {-400, 0, 35, 40, 40, 45};
   
   long long tot = 0;
   for(int i=0; i<6; ++i){
      tot += scr[i]*(popcnt(m.brd[ME] & rws[i])-popcnt(m.brd[HIM] & rws[i]));
   }
   return tot;
}

//evaluation of the board
//most important score is the distance of the minimum spanning tree between the groups
//the largest distance in this minimum spanning tree is a second heuristic
//a positive score is given for capturing other group to compensate for the loss of connection cost (most of the times these groups are unblocked later)
//a very small score is given to the amount of groups and the amount of moves available (helps choosing better moves when a lot of equal score moves available)
inline long long score(map_t &m){
   long long dis = 1100*(m.cst[HIM]-m.cst[ME]);
   long long max = 500*(m.max[HIM]-m.max[ME]);
   long long cap = 2000*(m.cap[HIM]-m.cap[ME]);
   long long mvs = 1*(popcnt(m.mvs[ME])-popcnt(m.mvs[HIM]));
   long long grp = 1*(popcnt(m.grep[HIM])-popcnt(m.grep[ME]));
   long long pos = pos_score(m);
   
   return dis + max + cap + mvs + grp + pos;
}

unsigned long long history[2][130][130];

pvs_t *pvs = 0;
map_t mp[30];

PLAYER pls;
map_t svm;
struct HistorySorter{
   inline bool operator()(uint128_t in1, uint128_t in2) const{
      uint128_t fr1 = in1 & svm.brd[pls];
      uint128_t fr2 = in2 & svm.brd[pls];
      if(history[pls][ctz(fr1)][ctz(fr1^in1)] == history[pls][ctz(fr2)][ctz(fr2^in2)]) return rand()%2;
      return (history[pls][ctz(fr1)][ctz(fr1^in1)] < history[pls][ctz(fr2)][ctz(fr2^in2)]);
   }
};
inline void delete_pvs(pvs_t *f){
   while(f){
      pvs_t *c = f;
      f = f->nxt;
      delete c;
   }
}

//negamax search
std::pair<long long, pvs_t*> negamax(map_t &m, int dpt, long long alpha, long long beta, int c, bool crit = false){
   bool quies = false;
   
   mp_reload(m);
   svm = m;
   PLAYER p = static_cast<PLAYER>(!(c+1));
   if(m.cst[p] == 0) return std::make_pair(MAX-40+dpt, nullptr);
   else if(dpt == 0) return std::make_pair(c*score(m), nullptr);
   
   //use quiescense search after 2/3 of the depth (but not in the first 6 depths, in the beginning of the game)
   else if(dpt < 2*lastdepth/3.0 && dpt < lastdepth-6) quies = true; 
   long long best = MIN; 
   uint128_t bm;
   pvs_t *f = 0;
   
   struct timeval before;
   if(crit) gettimeofday(&before, NULL);
   struct timeval after;

   int mcnt = 0;
   int trcnt = 0; //transposition
   int hmcnt = 0; //history
   uint128_t tmvs[150];
   uint128_t mvs[150];
   
   //load information from hash table
   hash_t r;
   HASHTYPE hm = tt_retrieve(r, m, dpt);
   #ifdef PVS
      if(hm == EXACT){
         alpha = r.score-1;
         beta = r.score+1;
      }
      else if(hm == LOWERBOUND && alpha < r.score-1) alpha = r.score-1;
      else if(hm == UPPERBOUND && r.score+1 < beta) beta = r.score+1;
      
      if(beta < alpha){
         return std::make_pair(r.score, nullptr);
      }else if(alpha == beta){
         alpha -= 1;
         beta  += 1;
      }
   #else
      if(hm == EXACT){
         pvs_t *pvs = nullptr;
         if(dpt == lastdepth) {
            pvs = new pvs_t;
            pvs->mv = r.bm; pvs->nxt = 0; 
         }
         return std::make_pair(r.score, pvs);
      }
      else if(hm == LOWERBOUND && alpha < r.score) alpha = r.score;
      else if(hm == UPPERBOUND && r.score < beta) beta = r.score;
      
      if(beta <= alpha) return std::make_pair(r.score, nullptr);
   #endif
   //update best moves
   if(hm != NOTFOUND){
      ++trcnt;
      mvs[0] = r.bm;
   }
   
   //quiescence search
   if(quies && m.cst[!p] != 0){
      long long stand_pat = c*score(m);
      if(stand_pat >= beta) return std::make_pair(beta, nullptr);
      else if(alpha < stand_pat) alpha = stand_pat;
   }
   
   //find all moves combinations
   unsigned int tgrep = m.grep[p];
   while(tgrep){
      unsigned int i = ctz(tgrep);
      tgrep ^= lsb(tgrep);
      uint128_t from = corners(m.grps[p][i]);
      uint128_t to = border(m.grps[p][i]) & m.mvs[p];
      while(to){
         uint128_t t = lsb(to);
         to ^= lsb(to);
         uint128_t tfrom = from;
         if(popcnt(m.grps[p][i]) != 1) tfrom &= ~border(t);
         while(tfrom){
            uint128_t f = lsb(tfrom);
            tfrom ^= f;
            tmvs[mcnt++] = f|t;
         }
      }
   }

   //sort the moves using history heuristic and use eventual transposition table first
   pls = p;
   std::priority_queue<uint128_t, std::vector<uint128_t>, HistorySorter> pq;
   for(int i=0; i<mcnt; ++i){
      if(pvs && tmvs[i] == pvs->mv){
         mvs[0] = tmvs[i];
         if (hm != NOTFOUND && r.bm != pvs->mv){
            mvs[1] = r.bm;
            ++trcnt;
         }else if(!trcnt) ++trcnt;
      }else pq.push(tmvs[i]);
   }
   hmcnt = pq.size();

   for(int i=0; i<hmcnt; ++i){
      mvs[trcnt+i] = pq.top();
      pq.pop();
   }
   
   #ifdef PVS
      if(pvs){
         pvs_t *c = pvs;
         pvs = pvs->nxt;
         delete c;
      }
   #endif
   
   //search all moves
   bool searchPV = true;
   long long origalpha = alpha;
   for(int i=0; i<mcnt; ++i){
      mp[dpt] = m;
      mp_apply(mp[dpt], mvs[i]);
      
      //use principal variation search for quicker converging
      std::pair<long long, pvs_t*> val;
      if(searchPV) {
         val = negamax(mp[dpt], dpt-1, -beta, -alpha, -c);
         val.first*=-1;
      }else{ 
         val = negamax(mp[dpt], dpt-1, -(alpha+1), -alpha, -c);
         val.first*=-1;
         if(alpha < val.first && val.first < beta){
            val = negamax(mp[dpt], dpt-1, -beta, -alpha, -c);
            val.first *= -1;
         }
      }
      
      if(best < val.first){ 
         best = val.first;    
         #ifdef PVS
            delete_pvs(f);
         #endif
         f = val.second;
         bm = mvs[i];
      }
      #ifdef PVS
         else delete_pvs(val.second);
      #endif
      if(alpha < val.first){
         alpha = val.first;
         searchPV = false;
      }
      if(beta <= alpha){
         //update history heuristic
         uint128_t fr = bm & m.brd[p];
         history[p][ctz(fr)][ctz(fr^bm)] += dpt*dpt;
         #ifdef PVS
            delete_pvs(f);
         #endif
         if(dpt != lastdepth) tt_store(m, best, bm, LOWERBOUND, dpt);
         return std::make_pair(best, nullptr);
      }else if(crit && (i%5) == 0){
         //cut the tree when too must time is spend searching
         gettimeofday(&after, NULL);
         double tm_before = before.tv_sec*1000.0+before.tv_usec/1000.0;
         double tm_after = after.tv_sec*1000.0+after.tv_usec/1000.0;
         if(2000 < tm_after - tm_before  || (23000 < elapsed_time && 500 < tm_after - tm_before)){
            delete_pvs(f);
            return std::make_pair(MIN+1, nullptr); 
         }
      }
   }
   //if no move can be found this is because not all pairs are generated and this results in no possible move in this situation --> we dont want to end here
   if(best == MIN){
      tt_store(m, MIN+2, 0, EXACT, dpt);
      return std::make_pair(MIN+2, nullptr);
   }

   if(best <= origalpha){
      if(dpt != lastdepth) tt_store(m, best, bm, UPPERBOUND, dpt);
      #ifdef PVS
         delete_pvs(f);
      #endif
      return std::make_pair(best, nullptr);
   }else{
      tt_store(m, best, bm, EXACT, dpt);
   
      #ifdef PVS
         pvs_t *pt = new pvs_t;
         pt->mv = bm;
         pt->nxt = f;
      #else
         pvs_t *pt = nullptr;
         if(dpt == lastdepth){
            pt = new pvs_t;
            pt->mv = bm;
            pt->nxt = f;
         }
      #endif
      return std::make_pair(best, pt);
   }
}

//iterated deepening around negamax
std::pair<long long, pvs_t*> iddfs(map_t m, PLAYER p){
   //use the previous heuristic history information less
   for(int i=0; i<2; ++i){
      for(int j=0; j<130; ++j){
         for(int k=0; k<130; ++k){
            history[i][j][k] /= 10; 
         }
      }
   }

   std::pair<long long, pvs_t*> ret;
   long long last[2] = {INT_MAX, INT_MAX};
   
   struct timeval before;
   struct timeval after;
   pvs = 0;
   
   int i=3;   
   bool mc = false;
   int c=1; if(p == HIM) c=-1;
   for(; i<30; ++i){
      lastdepth = i;
      gettimeofday(&before, NULL);
      double tm_before = before.tv_sec*1000.0+before.tv_usec/1000.0;
      std::pair<long long, pvs_t*> prs;
      
      //aspiration windows around the previous value
      if(last[i%2] == INT_MAX) prs = negamax(m, i, MIN, MAX, c, mc);
      else{
         prs = negamax(m, i, last[i%2]-2000, last[i%2]+2000, c, mc);
         if((prs.first != MIN+1) && (prs.second == nullptr || prs.first <= last[i%2]-2000 || last[i%2]+2000 <= prs.first)){
            #ifdef DEBUG
               std::cerr << "WRONG GUESS" << std::endl;
            #endif
            prs = negamax(m, i, MIN, MAX, c, mc);
         }
      }
      
      if(prs.second != nullptr) ret = prs;
      else{
         #ifdef DEBUG
            std::cerr << "CUTOFF" << std::endl;
         #endif
         lastdepth = i-1;
         break;
      }
      last[i%2] = prs.first;
      pvs_t *tmp = pvs = new pvs_t;
      pvs_t *cp = ret.second;
      while(cp){
         tmp->mv = cp->mv;
         if(cp->nxt){
            tmp->nxt = new pvs_t;
            tmp = tmp->nxt;
         }else tmp->nxt = 0;
         cp = cp->nxt;
      }
      gettimeofday(&after, NULL);
      double tm_after = after.tv_sec*1000.0+after.tv_usec/1000.0;
      if(MAX-50 < ret.first || ret.first < MIN+50 || (160 < tm_after - tm_before) || (23000 < elapsed_time && 50 < tm_after - tm_before)) break;
      if(50 < tm_after - tm_before) mc = true;
   }
   delete_pvs(pvs);
#ifdef DEBUG
   std::cerr << "DEPTH " << lastdepth << std::endl;
#endif
   return ret;
}

/** COMMUNICATION **/
int main(){
   std::cerr << "R AYUKF" << std::endl;
   for(int i=0; i<2; ++i){
      for(int j=0; j<130; ++j){
         for(int k=0; k<130; ++k){
            history[i][j][k] = 0;
         }
      }
   }
   
   //always use the same hashes for zobrist key
   std::srand (0xcafebabe); 
   for(int i=0; i<250; ++i){
      mp_zobrist[i] = rand()%TT_SIZE;
   }
   for(int i=0; i<TT_SIZE; ++i){
      tt_tab1[i].brd[0] = tt_tab1[i].brd[1] = tt_tab1[i].bm = 0;
      tt_tab2[i].brd[0] = tt_tab2[i].brd[1] = tt_tab2[i].bm = 0;
   }
   std::srand ( unsigned ( std::time(0) ) ); 

   //check if were first
   std::string s;
   std::cin >> s;
   if(s == "Start"){
      //we do the first move
      map = mp_init(ME);
      
      //apply the first move
      std::string mv = "B1-B2";
      mp_apply(map, mv);
      std::cout << mv << std::endl;
      #ifdef DEBUG
      std::cerr << "MOVE " << mv << std::endl;
      #endif
   }else{
      //they do the first move   
      map = mp_init(HIM);
      
      //apply their move
      mp_apply(map, s);
      #ifdef DEBUG
      std::cerr << "MOVE " << s << std::endl;
      #endif
      
      //apply a reply move
      mp_reload(map);
      std::string mv = "A2-B2";
      if(map.brd[HIM] & (uint128_1 << 12)) mv = "K10-J10";
      mp_apply(map, mv);
      #ifdef DEBUG
      std::cerr << "MOVE " << mv << std::endl;
      #endif
      std::cout << mv << std::endl;
   }

   //init
   mp_reload(map);

   //continue the game
   while(true){
      std::string in;
      std::cin >> in; 
      if(in == "Quit") break;
      
      //apply their move
      #ifdef DEBUG
      std::cerr << "MOVE " << in << std::endl;
      #endif
      mp_apply(map, in);
      
      //reload the map distances
      mp_reload(map);
      
      //generate the best move; 
      struct timeval before, after;
      gettimeofday(&before, NULL);
      double tm_before = before.tv_sec*1000.0+before.tv_usec/1000.0;
      std::pair<long long, pvs_t*> val = iddfs(map, ME);
      uint128_t f = val.second->mv & map.brd[ME];
      uint128_t t = val.second->mv ^ f;
      std::string mv = convert(f);
      mv+="-";
      mv+=convert(t);
      #ifdef DEBUG
         std::cerr << "DEBUG " << val.first << " " << mv << " " << map.cst[ME] << " " << map.cst[HIM] << std::endl;
         std::cerr << "MOVE " << mv << std::endl;
         pvs_t *c = val.second;
         map_t tmap = map;
         PLAYER cur = ME;
         std::cerr << "EXPECT ";
         while(c){
            uint128_t f = c->mv & tmap.brd[cur];
            uint128_t t = c->mv ^ f;
            cur = static_cast<PLAYER>(!cur);
            std::cerr << convert(f) << "-" << convert(t);
            mp_apply(tmap, c->mv);
            mp_reload(tmap);
            c = c->nxt;
            if(c) std::cerr << " -> ";
         }
         std::cerr << std::endl;
         std::cerr << "HASH " << map.hash << std::endl;
      #endif
      mp_apply(map, val.second->mv);
      delete_pvs(val.second);
      move_cnt += 2;
      
      std::cout << mv << std::endl;
      gettimeofday(&after, NULL);
      double tm_after = after.tv_sec*1000.0+after.tv_usec/1000.0;
      elapsed_time += tm_after-tm_before;
      #ifdef DEBUG
      std::cerr << "ELAPSED " << elapsed_time << std::endl;
      #endif
   }
}
