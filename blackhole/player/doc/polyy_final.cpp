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

#define MAX_S 1000000
#define MIN_S -1000000

int SPEC = false;

enum Player {ME = 0, HIM = 1, EMPTY = 2};
//Player ME;
//Player HIM;	

short killer[2];

int MAX_D = 1;

/** STRUCTURES **/
struct Board{
	Board() {hash1[0] = hash1[1] = 0; hash2[0] = hash2[1] = 0;}
	uint64_t hash1[2];
	uint64_t hash2[2];
	int blk_cnt;
};
Board brd;
Player table[110];

struct Node{
	Node(): m(0), h(0), d(0), w(0), n(1) { for(int i=0; i<106; ++i) c[i] = 0, mv[i] = true; }
	
	Player p; //player
	int m; //move
	int h; //heuristic
	int d; //depth
	
	int b; //last best move
	int w; //total wins
	int n; //total games
	bool mv[106];
	Node *c[106];
};
Node *r = 0;

bool operator==(Board b1, Board b2){
	return (b1.hash1[0] == b2.hash1[0] && b1.hash2[0] == b2.hash2[0] && b1.hash1[1] == b2.hash1[1] && b1.hash2[1] == b2.hash2[1]);
}

int blk_cnt = 0;

/** HELP **/
int mod(int a, int b){
	return ((a+b-1) % b)+1;
}

/** DEBUG **/
void dbg(std::string s){
#ifdef DEBUG
	std::cerr << "==> " << s << std::endl;
#endif
}

/** GAME **/
inline Player get_player(Board &b, int po){
	if(po<64){
		uint64_t chk = (1ll << po);
		if(b.hash1[0] & chk) return ME;
		else if(b.hash1[1] & chk) return HIM;
		else return EMPTY;
	}else{
		uint64_t chk = (1ll << (po-64));
		if(b.hash2[0] & chk) return ME;
		else if(b.hash2[1] & chk) return HIM;
		else return EMPTY;
	}
}

inline void place(Board &b, Player pl, int po){
	++b.blk_cnt;
	if(po<64) b.hash1[pl] ^= (1ll << po);
	else b.hash2[pl] ^= (1ll << (po-64));
}

inline void pickup(Board &b, Player pl, int po){
	--b.blk_cnt;
	if(po<64) b.hash1[pl] ^= (1ll << po);
	else b.hash2[pl] ^= (1ll << (po-64));
}

inline void place_final(Board &b, Player pl, int po){
	place(b, pl, po);
	++blk_cnt;
	if(pl == ME || SPEC) std::cout << po << std::endl;
}

std::vector<int> get_moves(Board &b){
	std::vector<int> ret;
	for(int i=1; i<64; ++i){
		if(((b.hash1[0] | b.hash1[1]) & (1ll << i)) == false){
			ret.push_back(i);
		}
	}
	for(int i=64; i<=106; ++i){
		if(((b.hash2[0] | b.hash2[1]) & (1ll << (i-64))) == false){
			ret.push_back(i);
		}
	}
	return ret;
}

std::vector<int> get_grid_moves(Board &b){
	std::vector<int> ret;
	for(int i=0; i<15; ++i){
		if(grid[i] < 64 && ((b.hash1[0] | b.hash1[1]) & (1ll << grid[i])) == false){
			ret.push_back(grid[i]);
		}else if(grid[i] >= 64 && ((b.hash2[0] | b.hash2[1]) & (1ll << (grid[i]-64))) == false){
			ret.push_back(grid[i]);
		}
	}
	return ret;
}

std::vector<int> get_neighbours(int p){
	std::vector<int> vc;
	for(int i=0; i<6; ++i){
		if(nghbrs[p][i] != 0) vc.push_back(nghbrs[p][i]);
	}
	return vc;
}

inline std::vector<int> get_neighbours_far(int p){
	std::vector<int> vc;
	for(int i=0; i<6; ++i){
		if(nghbrs[p][i] != 0) vc.push_back(nghbrs_far[p][i]);
	}
	return vc;
}

inline std::pair<int, int> get_neighbours_cnt(Board &b, Player pl, int p){
	std::pair<int, int> pa = std::make_pair(0, 0);
	for(int i=0; i<6; ++i){
		Player pl2 = get_player(b, nghbrs[p][i]);
		if(pl2 == pl) ++pa.first;
		else if(pl2 == !pl) ++pa.second;
	}
	return pa;
}

int bdis[107];
int bcst[107];
std::vector<int> dijkstra(Board &b, Player pl){
	std::vector<int> vec(7, MAX_S);

	int to[6];
	for(int i=1; i<6; ++i){
		to[i] = side[i][0];
	}
	
	for(int i=1; i<=106; ++i){
		int j = i;
		if(get_player(b, j) == (!pl)) continue;
		
		int reach[6];
		for(int k=0; k<6; ++k) reach[k] = MAX_S;
		
		for(int i=1; i<=106; ++i) {
			bdis[i] = MAX_S;
			
			Player p = get_player(b, i);
			if(p == pl) bcst[i] = 0;
			else if(p == (!pl)) bcst[i] = MAX_S;
			else {
				bcst[i] = 2;//+get_neighbours_cnt(b, pl, i).second;
			}
		}
		std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, std::greater<std::pair<int, int> > > pq;
		bdis[j] = 0;
		pq.push(std::make_pair(0, j));
		
		while(!pq.empty()){
			std::pair<int, int> p = pq.top();
			pq.pop();
			
			if(p.first != bdis[p.second]) continue;
			std::vector<int> ngh = get_neighbours_far(p.second);
			
			for(int i=0; i<ngh.size(); ++i){
				if(ngh[i] < 0){
					if((get_player(b, between_side[p.second][-ngh[i]][0]) != (!pl) || get_player(b, between_side[p.second][-ngh[i]][1]) != (!pl)) && bdis[p.second] < reach[-ngh[i]]){
						if(get_player(b, between_side[p.second][-ngh[i]][0]) == pl || get_player(b, between_side[p.second][-ngh[i]][1]) == pl){
							reach[-ngh[i]] = bdis[p.second];
							if(get_player(b, between_side[p.second][-ngh[i]][0]) == pl) to[-ngh[i]] = between_side[p.second][-ngh[i]][0];
							else to[-ngh[i]] = between_side[p.second][-ngh[i]][1];
						}else if(get_player(b, between_side[p.second][-ngh[i]][0]) == (!pl) || get_player(b, between_side[p.second][-ngh[i]][1]) == (!pl)){
							reach[-ngh[i]] = bdis[p.second]+3;
							if(get_player(b, between_side[p.second][-ngh[i]][0]) == (!pl)) to[-ngh[i]] = between_side[p.second][-ngh[i]][1];
							else to[-ngh[i]] = between_side[p.second][-ngh[i]][0];
						}else{
							reach[-ngh[i]] = bdis[p.second]+1;
							to[-ngh[i]] = between_side[p.second][-ngh[i]][0]; //random??
						}
					}
				}else if((get_player(b, between[p.second][ngh[i]][0]) != (!pl) || get_player(b, between[p.second][ngh[i]][1]) != (!pl)) && get_player(b, p.second) != (!pl)){
					int f_cst = p.first+bcst[ngh[i]];
					
					if(get_player(b, between[p.second][ngh[i]][0]) == pl || get_player(b, between[p.second][ngh[i]][1]) == pl){
					}else if(get_player(b, between[p.second][ngh[i]][0]) == (!pl) || get_player(b, between[p.second][ngh[i]][1]) == (!pl)){
						f_cst += 3;
					}else{
						f_cst += 1;
					}
					if(bdis[p.second] + f_cst >= bdis[ngh[i]]) continue;
					
					bdis[ngh[i]] = f_cst;
					pq.push(std::make_pair(bdis[ngh[i]], ngh[i]));
				}
			}
			
			ngh = get_neighbours(p.second);
			for(int i=0; i<ngh.size(); ++i){
				if(ngh[i] < 0){
					if(bdis[p.second] < reach[-ngh[i]]){
						reach[-ngh[i]] = bdis[p.second];
						to[-ngh[i]] = p.second;
					}
				}
				if(get_player(b, ngh[i]) == pl && get_player(b, p.second) == pl && bdis[p.second] < bdis[ngh[i]]) {
					bdis[ngh[i]] = bdis[p.second];
					pq.push(std::make_pair(bdis[ngh[i]], ngh[i]));
				}else if(get_player(b, ngh[i]) != (!pl) && bdis[p.second]+2 < bdis[ngh[i]]){
					bdis[ngh[i]] = bdis[p.second]+2;
					pq.push(std::make_pair(bdis[ngh[i]], ngh[i]));
				}
			}
		}
		
		int total = 0;
		int mn = 0;
		int mni = -1;
		for(int k=1; k<=5; ++k){
			if(reach[k] > mn) {
				mn = reach[k];
				mni = k;
			}
			total += reach[k];
		}
		total -= mn;
		
		if(total < vec[0]){
			vec[0] = total;
			vec[1] = j;
			int ci = 2;
			for(int k=1; k<=5; ++k){
				if(k==mni) continue;
				if(reach[k] == MAX_S){
					std::cerr << "!!" << std::endl;
					vec[0] = -1;
					return vec;
				}
				vec[ci] = to[k%5+1];
				++ci;
			}
		}
	}
	return vec;
	
}

int frm[107];
int dis[107];
int cst[107];
std::pair<int, int> get_move(Board &b, Player pl, int from, int to, std::vector<int> &lst){
	for(int i=1; i<=106; ++i) {
		dis[i] = MAX_S;
		frm[i] = i;
		
		Player p = get_player(b, i);
		if(p == pl) cst[i] = 0;
		else if(p == (!pl)) cst[i] = MAX_S;
		else {
			cst[i] = 2;//+get_neighbours_cnt(b, pl, i).second;
		}
	}
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, std::greater<std::pair<int, int> > > pq;
	dis[from] = 0;
	pq.push(std::make_pair(0, from));

	while(dis[to] == MAX_S){
		if(pq.empty()) break;
		std::pair<int, int> p = pq.top();
		pq.pop();
		
		if(p.first != dis[p.second]) continue;
		std::vector<int> ngh = get_neighbours_far(p.second);
		
		for(int i=0; i<ngh.size(); ++i){
			if(ngh[i] < 0){
				//
			}else if((get_player(b, between[p.second][ngh[i]][0]) != (!pl) || get_player(b, between[p.second][ngh[i]][1]) != (!pl)) && get_player(b, p.second) != (!pl)){
				int f_cst = p.first+cst[ngh[i]];
				
				if(get_player(b, between[p.second][ngh[i]][0]) == pl || get_player(b, between[p.second][ngh[i]][1]) == pl){
				}else if(get_player(b, between[p.second][ngh[i]][0]) == (!pl) || get_player(b, between[p.second][ngh[i]][1]) == (!pl)){
					f_cst += 3;
				}else{
					f_cst += 1;
				}
				if(dis[p.second] + f_cst >= dis[ngh[i]]) continue;
				
				dis[ngh[i]] = f_cst;
				frm[ngh[i]] = p.second;
				pq.push(std::make_pair(dis[ngh[i]], ngh[i]));
			}
		}
		
		ngh = get_neighbours(p.second);
		for(int i=0; i<ngh.size(); ++i){
			if(ngh[i] < 0){
				//
			}
			if(get_player(b, ngh[i]) == pl && get_player(b, p.second) == pl && dis[p.second] < dis[ngh[i]]) {
				dis[ngh[i]] = dis[p.second];
				frm[ngh[i]] = p.second;
				pq.push(std::make_pair(dis[ngh[i]], ngh[i]));
			}else if(get_player(b, ngh[i]) != (!pl) && dis[p.second] + 2 < dis[ngh[i]]){
				dis[ngh[i]] = dis[p.second]+2;
				frm[ngh[i]] = p.second;
				pq.push(std::make_pair(dis[ngh[i]], ngh[i]));
			}
		}
	}
	
	std::pair<int, int> p = std::make_pair(0, 0);
	int other = -1;
	
	int bef = lst.size();
	//std::cerr << "distance: " << dis[to] << std::endl;
	//std::cerr << "move list: ";
	
	//std::vector<int> lst;
	while(frm[to] != to){
		if(get_player(b, to) == EMPTY) lst.push_back(to);
		//std::cerr << to << " (" << dis[to]-dis[frm[to]] << ") ";
		
		if(get_player(b, to) == EMPTY) {
			++p.second;
			other = to;
		}
		
		if(between[frm[to]][to][0] == 0) {
			to = frm[to];
			continue;
		}
		
		if(other == -1 && get_player(b, between[frm[to]][to][0]) == EMPTY) other = between[frm[to]][to][0];
		if(other == -1 && get_player(b, between[frm[to]][to][1]) == EMPTY) other = between[frm[to]][to][1];
		
		if(get_player(b, between[frm[to]][to][0]) == pl || get_player(b, between[frm[to]][to][1]) == pl);
		else if(get_player(b, between[frm[to]][to][0]) == (!pl) || get_player(b, between[frm[to]][to][1]) == (!pl)){
			p.second+=150;
			if(get_player(b, between[frm[to]][to][0]) == (!pl)) {
				lst.push_back(between[frm[to]][to][1]);
				p.first = between[frm[to]][to][1];
			}else{
				lst.push_back(between[frm[to]][to][0]);
				p.first = between[frm[to]][to][0];
			}
		}
		
		to = frm[to];
	}
	if(get_player(b, to) == EMPTY) lst.push_back(to);
	
	if(get_player(b, to) == EMPTY) other = to;
	
	if(p.second < 150) p.first = other;
	if(lst.size()-bef == 0 && other != -1) lst.push_back(other);
	//std::cerr << "choosing move: " << p.first << std::endl;
	
	return p;
}

std::pair<std::pair<int, int>, std::vector<int> > get_best_moves(Board &b, Player p){
	std::vector<int> vc = dijkstra(b, p);
	
	if(vc[0] == -1 || vc[0] >= MAX_S) return std::make_pair(std::make_pair(0, MAX_S), std::vector<int>());
	
	//std::cerr << "best place is " << vc[1] << " (" << vc[0] << "): (" << vc[2] << "," << vc[3] << ") - (" << vc[4] << "," << vc[5] << ") (" << vc[6] << "," << vc[7] << ") (" << vc[8] << "," << vc[9] << ") " << std::endl; 
	std::cerr << "best place is " << vc[1] << " (" << vc[0] << "): " << vc[2] << " " << vc[3] << " " << vc[4] << " " << vc[5] << std::endl;
			
	
	std::vector<int> mvs;
	std::pair<int, int> pr[4];
	int mx = -1;
	int ind = -1;
	pr[0] = get_move(b, ME, vc[1], vc[2], mvs);
	pr[1] = get_move(b, ME, vc[1], vc[3], mvs);
	pr[2] = get_move(b, ME, vc[1], vc[4], mvs);
	pr[3] = get_move(b, ME, vc[1], vc[5], mvs);
	for(int i=0; i<4; ++i){
		if(pr[i].first > 0 && pr[i].second > mx){
			mx = pr[i].second;
			ind = i;
		}
	}

	std::sort(mvs.begin(), mvs.end());
	std::vector<int>::iterator it = std::unique(mvs.begin(), mvs.end());
	mvs.resize(it-mvs.begin());
	
	if(ind == -1) return std::make_pair(std::make_pair(0, MAX_S), mvs);
	else return std::make_pair(std::make_pair(pr[ind].first, vc[0]), mvs);
}

bool ffill[107];
int reach[6];
int done[6];
inline Player check_win(Board &b, Player p){
	for(int i=1; i<=106; ++i) ffill[i] = false;
	for(int i=1; i<6; ++i) done[i] = false;
	
	int cnt = 0;
	for(int i=1; i<6; ++i){
		for(int j=0; j<7; ++j){
			if(get_player(b, side[i][j]) == p && ffill[side[i][j]] == false){
				for(int k=1; k<6; ++k) reach[k] = false;
				ffill[side[i][j]] = true;
				
				int scnt = 0;
				int stk[106];
				stk[scnt] = side[i][j];
				while(scnt >= 0){
					int z = stk[scnt];
					--scnt;
					for(int k=0; k<6; ++k){
						if(nghbrs[z][k] == 0) break;
						else if(nghbrs[z][k] < 0) reach[-nghbrs[z][k]] = true;
						else if(ffill[nghbrs[z][k]] == false && get_player(b, nghbrs[z][k]) == p){
							ffill[nghbrs[z][k]] = true;
							++scnt;
							stk[scnt] = nghbrs[z][k];
						}
					}
				}
				
				int chk = 0;
				int cond = 0;
				for(int k=1; k<6; ++k){
					if(reach[k] && reach[(k%5)+1]) ++cond;
					if(reach[k]) ++chk;
				}
				if(chk >= 4) return p;
				if(cond && chk == 3) cnt += cond;
				
				if(cnt >= 3) return p;
			}
		}
	}
	return static_cast<Player>(!p);
}

int heuristic_score(Board b, Player pl){
	int score = 0;
	int cnt[2]; 
	
	for(int i=1; i<=106; ++i){
		Player pp = get_player(b, i);
		if(pp == EMPTY) continue;
	
		cnt[0] = cnt[1] = 0;
		std::vector<int> ngh = get_neighbours_far(i);
		
		for(int j=0; j<ngh.size(); ++j){
			if(ngh[j] < 0) continue;
			
			Player p = get_player(b, ngh[j]);
			if(p == pp) {
				if(get_player(brd, between[i][ngh[j]][0]) == EMPTY && get_player(brd, between[i][ngh[j]][1]) == EMPTY) cnt[pp] += 6;
				else if((get_player(brd, between[i][ngh[j]][0]) == pp) != (get_player(brd, between[i][ngh[j]][1]) == pp)) cnt[pp] += 4;
			}else if(p == EMPTY){
				if((get_player(brd, between[i][ngh[j]][0]) == pp) != (get_player(brd, between[i][ngh[j]][1]) == pp)) cnt[pp] += 1;
			}
		}
		
		if(pp == pl) score += cnt[pp];
		else score -= cnt[pp];
	}
	return score;
}


#define EPS 1e-8
#define MCTS_EXPL 0.5
#define MCTS_ITER 8000
#define MCTS_NO_AMAF 10
#define MCTS_HEURISTIC 1
Player fin_use[107];
bool good_move[2][107];
inline Player mcts_finish(Board b, Player c, Player w){
	std::vector<int> m_arr = get_moves(b);
	std::random_shuffle(m_arr.begin(), m_arr.end());
	for(int i=0; i<m_arr.size(); ++i){
		place(b, c, m_arr[i]);
		fin_use[m_arr[i]-1] = c;
		c = static_cast<Player>(!c);
	}
	return check_win(b, w);
}

std::pair<int, Node*> mcts_expand(Board &bd, Player p, int m){
	place(bd, p, m);
	
	Node *c = new Node;
	c->p = p;
	c->m = m;
	c->d = bd.blk_cnt;
	for(int i=0; i<106; ++i){
		if(get_player(bd, i+1) != EMPTY) c->mv[i] = false;
	}
	
	if(mcts_finish(bd, static_cast<Player>(!p), p) == p) c->w = MCTS_NO_AMAF;
	else c->w = 0;
	c->n = 1;
	if(good_move[p][m]){
		//std::cout << m << std::endl;
		c->h = 200;
	}else if(good_move[!p][m]){
		c->h = 100;
	}else c->h = 0;
	
	pickup(bd, p, m);
	return std::make_pair(!c->w, c);
}

std::pair<int, Node*> mcts_select(Board &bd, Player p, Node *c){
	if(c->m != 0) place(bd, p, c->m);
	
	double max = INT_MIN;
	Node *b = 0;
	int m = -1;
	for(int i=0; i<106; ++i){
		if(!c->mv[i]) continue;
		
		double score = 0;
		if(c->c[i]){
			//score += (1-c->am)*c->c[i]->w/(c->c[i]->n + EPS) + c->am*c->c[i]->aw/(c->c[i]->an + EPS) 
			score += c->c[i]->w/(c->c[i]->n + EPS);
			score += MCTS_EXPL*sqrt(log(c->n) / (c->c[i]->n + EPS));
			score += MCTS_HEURISTIC*c->c[i]->h / (c->c[i]->n + EPS);
			//std::cout << c->c[i]->h / (c->c[i]->n + EPS) << std::endl;
		}else{
			score += MCTS_EXPL*sqrt(log(c->n) / (EPS));
		}
		
		score += rand()*EPS;
		if(score > max){
			max = score;
			b = c->c[i];
			m = i;
		}
	}
	
	std::pair<int, Node*> ret;
	if(b == 0) ret = mcts_expand(bd, static_cast<Player>(!p), m+1);
	else ret = mcts_select(bd, static_cast<Player>(!p), b);
	
	c->c[m] = ret.second;
	c->w += MCTS_NO_AMAF*ret.first;
	c->b = m;
	c->n += MCTS_NO_AMAF;
	
	for(int i=0; i<106; ++i){
		if(c->c[i] && fin_use[i] == static_cast<Player>(!p)){
			c->c[i]->w += !ret.first;
			c->c[i]->n++;
		}
	}
	
	if(c->m != 0) pickup(bd, p, c->m);
	return std::make_pair(!ret.first, c);
}

int mcts_simulate(Board &bd, Player p, Node *c){
	int chs[107];
	for(int i=0; i<107; ++i) chs[i] = 0;
	
	for(int i=0; i<MCTS_ITER; ++i){
		std::pair<int, Node *> pr = mcts_select(bd, p, c);
		++chs[pr.second->b+1];
	}
	int max = 0;
	int mm = 0;
	std::vector<std::pair<double, int>> v;
	for(int i=1; i<107; ++i){
		if(c->c[i-1]) v.push_back(std::make_pair(/*static_cast<double>(c->c[i-1]->w)/c->c[i-1]->n*/ c->c[i-1]->n , i));
		if(chs[i] > max){
			max = chs[i];
			mm = i;
		}
	}
	
	std::sort(v.begin(), v.end(), std::greater<std::pair<double, int> >());
	for(int i=0; i<10; ++i){
		std::cerr << " >>> " << v[i].second << ": " << v[i].first << " - " << static_cast<double>(c->c[v[i].second-1]->w)/c->c[v[i].second-1]->n << std::endl;
	}
	return mm;
}

void delete_node(Node *c){
	if(c == 0) return;
	for(int i=0; i<106; ++i){
		delete_node(c->c[i]);
	}
	delete c;
}

Node *mcts_lower(Node *c, int m){
	for(int i=0; i<106; ++i){
		if(i != m-1){
			delete_node(c->c[i]);
		}
	}
	Node *n = c->c[m-1];
	delete c;
	
	return n;
}

/** COMMUNICATION **/
int main(int argc, char **argv){
	//init
	std::srand ( unsigned ( std::time(0) ) );
	if(argc > 1) std::cout << "R PYK1" << std::endl;
	else std::cerr << "R PYK1" << std::endl;
	killer[0] = killer[1] = 0;
	
	/*ME = ME;
	HIM = HIM;
	
	SPEC = true;
	monte_carlo(brd);
	return 0;*/
	/*place_final(brd, ME, 1);
	place_final(brd, HIM, 81);
	place_final(brd, ME, 2);
	place_final(brd, HIM, 3);
	place_final(brd, ME, 5);
	place_final(brd, HIM, 9);
	place_final(brd, ME, 100);
	place_final(brd, HIM, 16);
	place_final(brd, ME, 105);
	place_final(brd, HIM, 25);
	place_final(brd, ME, 101);
	place_final(brd, HIM, 36);
	place_final(brd, ME, 95);
	place_final(brd, HIM, 49);
	place_final(brd, ME, 45);
	place_final(brd, HIM, 6);
	place_final(brd, ME, 44);
	place_final(brd, HIM, 11);
	place_final(brd, ME, 32);
	//place_final(brd, HIM, 10);*/
	//SPEC = true;
	//ME = ME;
	//HIM = HIM;
	/*place_final(brd, ME, 40);
	place_final(brd, HIM, 12);
	place_final(brd, ME, 35);
	place_final(brd, HIM, 68);
	place_final(brd, ME, 7);
	place_final(brd, HIM, 43);
	place_final(brd, ME, 42);
	place_final(brd, HIM, 64);
	place_final(brd, ME, 71);
	place_final(brd, HIM, 6);
	place_final(brd, ME, 3);
	place_final(brd, HIM, 30);
	place_final(brd, ME, 59);
	place_final(brd, HIM, 20);
	place_final(brd, ME, 22);
	place_final(brd, HIM, 34);
	place_final(brd, ME, 47);
	place_final(brd, HIM, 88);
	place_final(brd, ME, 67);
	place_final(brd, HIM, 41);
	place_final(brd, ME, 21);
	place_final(brd, HIM, 106);*/
	
	Node *r = new Node;
	
	std::string s;
	std::cin >> s;
	int init_m = -1;
	if(s == "Start"){
		std::vector<int> vec = get_grid_moves(brd);
		init_m = 56;//vec[rand()%vec.size()];
		place_final(brd, ME, init_m);
	}else{
		//do we want to swap
		if(false){
			std::cout << -1 << std::endl;
		}else{
			place_final(brd, HIM, atoi(s.c_str()));
			
			std::vector<int> vec = get_grid_moves(brd);
			place_final(brd, ME, vec[rand()%vec.size()]);
		}
	}
	
	while(true){
		int in;
		std::cin >> in; 
		
		if(in == -1) {
			dbg("swap");
			
			pickup(brd, ME, init_m);
			place(brd, HIM, init_m);
			
			std::vector<int> vec = get_grid_moves(brd);
			place_final(brd, ME, vec[rand()%vec.size()]);
			
			continue;
		}else if(in == 0 /* QUIT */) break;
		
		place_final(brd, HIM, in);
		
		r = mcts_lower(r, in);
		if(r == 0){
			r = new Node;
			for(int i=1; i<=106; ++i){
				if(get_player(brd, i) != EMPTY){
					r->mv[i-1] = false;
				}
			}
		}else r->m = 0;
		
		for(int i=1; i<=106; ++i) good_move[0][i] = good_move[1][i] = false; 
		std::vector<int> vec = get_best_moves(brd, ME).second;
		for(int i=0; i<vec.size(); ++i){
			good_move[ME][vec[i]] = true;
			std::cerr << "-> " << vec[i] << std::endl;
		}
		vec = get_best_moves(brd, HIM).second;
		for(int i=0; i<vec.size(); ++i){
			good_move[HIM][vec[i]] = true;
		}
		
		int m = mcts_simulate(brd, HIM, r);
		std::cerr << "--> " << m << std::endl;
		place_final(brd, ME, m);
		r = mcts_lower(r, m);
	}
	
	delete_node(r);
}
