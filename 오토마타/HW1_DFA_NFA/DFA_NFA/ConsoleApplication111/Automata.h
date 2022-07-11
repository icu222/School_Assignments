#ifndef _Automata_h

#define _Automata_h


#include <iostream>
#include <string>
#include <algorithm>


using namespace std;


//============eNFA =====================
const int enfa_state = 11;	// 다른 FA 실험시, 여기 const int의 정보를 수정해야 합니다.
const int enfa_of_input = 2;
const int enfa_trans_func = 13;
const int max_end_state = 10;

//=================DFA==================
int dfa_trans_func = 0;
int dfa_state = 0;
int dfa_end_state = 0;

//==================NFA====================
// eNFA, DFA와 관련 없는 다른 NFA입니다. 3주차 6페이지에 있는 오토마타입니다. 
const int nfa_state = 4;
const int nfa_input = 2;
const int nfa_trans_f = 8;
const int nfa_ed_state = 1;

// ================== queue ==================
int head = 0;
int tail = 0;


/*======== 오토마타의 구조체 ======     */
// eNFA, DFA, NFA 모두 transition func만 다른 것을 제외하면
//  기본적으로 같기 때문에 같은 구조체로 구현하였습니다. 

typedef struct {
	string src_state;
	string input;
	string dst_state;

}trans_func;


typedef struct {
	string start_state;

	// end state를 10개라고 가정
	string end_state[max_end_state];

	// input은 2개라고 가정 ( 앱실른 제외 )
	string input[2];

	string* state;

	trans_func* trans_f;


} Automata;


/* =========== 큐 함수 ==================*/
void push(string s[], string input_str) {
	s[tail++] = input_str;
}

string pop(string* s) {
	return s[head++];
}


//==================== 함수 ==========================

/* 큐에 해당 스트링이 존재하는지 확인 */
bool not_in_queue(string q[], string s) {
	// tail이 있으므로 길이 인자를 넣지 안아도 된다. 
	bool result = true;

	//tail이 큐의 마지막 스트링을 가리키고 있으므로
	for (int x = 0; x <= tail; x++)
		if (q[x] == s)
			result = false;

	return result;

}


/*  주어진 state(ex : ACFG)가 end state를 하나라도 포함하는지 확인하는 함수 */
bool is_contain_end_state(string str, string end[], int end_len) {

	for (unsigned int t = 0; t < str.length(); t++) {
		for (int y = 0; y < end_len; y++) {

			// 즉 end state를 포함하면 포함한 위치(index)를 반환하므로 그 안에 있으면 true
			// 이상하게 빈 스트링을 find에 넣으면 0이 나와서 이를 참이라고 인식해서 추가 조건 end[y].length() != 0 삽입
			if (str.find(end[y]) >= 0 && str.find(end[y]) < str.length() && end[y].length() != 0)
				return true;
		}
	}


	return false;
}

/* 주어진 state의 closure를 구하여 result에 저장 */
void Closure(Automata a, string src, string* result) {


	int not_in_result = 1;

	// 처음 state 가 문자 하나일때
	// 즉 이게 계속 호출되니깐 ABCD => A => TF(A,@) = C일때, result에 A만 있는 상태
	// 그러나 언젠가 result에 저장된 애가 들어올 것이다. 
	if (src.length() == 1) {

		for (unsigned int y = 0; y < (*result).length(); y++) {
			if ((*result).substr(y, 1) == src)
				not_in_result = 0;
		}
		if (not_in_result) {

			(*result).append(src);
		}
	}

	// input state 이름이 abvdf이렇게 길 때
	else {
		for (unsigned int e = 0; e < src.length(); e++) {
			// 일단 다 넣어 그게 closure니깐
			(*result).append(src.substr(e, 1));
		}

	}

	// 만약 현재 state에서 @일 때 갈 수 있는 state가 존재한다면
	for (unsigned int f = 0; f < src.length(); f++) {
		for (unsigned int r = 0; r < enfa_trans_func; r++) {
			if (a.trans_f[r].src_state == src.substr(f, 1) && a.trans_f[r].input == "@") {
				// 그 상태에 대해 closure 실행
				Closure(a, a.trans_f[r].dst_state, result);
			}
		}
	}

	sort((*result).begin(), (*result).end());

}

/* src_state,input ------use transition_function --------->> find dst_state  */
string transition_output(Automata a, string src_state, string input, int num_tf) {

	string res;
	int t = 0;
	for (unsigned int f = 0; f < src_state.length(); f++) {
		for (int r = 0; r < num_tf; r++) { 
			// 즉 주어진 string의 원소(상태) 와 input에 대한 transition이 존재한다면
			if (a.trans_f[r].src_state == src_state.substr(f, 1) && a.trans_f[r].input == input)
				// 그 output state를 저장
				res.append(a.trans_f[r].dst_state);
		}
	}
	// 저장된 output state들을 반환
	return res;

}

/* eNFA ---> DFA */
Automata Subset_Construction(Automata a) {

	Automata result_dfa;
	result_dfa.input[0] = a.input[0];
	result_dfa.input[1] = a.input[1];

	// alloc은 미친짓이다 그냥 만들고 옮겨라
	trans_func res_trans_func[64];

	// 결과 저장용 스트링
	string result_state0;
	string result_state1;
	string poped_str;

	string queue[1 << enfa_state]; // 만약 상태가 6개라면 최대 2^6 = 64개이므로


	// eNFA -> DFA 1단계 : 시작state의 closure가 시작상태이다.  
	// 시작 상태를 result_state0에 저장
	Closure(a, a.start_state, &result_state0);
	push(queue, result_state0);

	result_dfa.start_state = result_state0;

	// eNFA -> DFA 2단계 : 이제 본격적인 subset construction 시작
	while (head != tail) {

		result_state0.clear();
		result_state1.clear();

		// 이것을 꺼내서 
		poped_str = pop(queue);

		// input은 2개라 가정했으므로 
		Closure(a, transition_output(a, poped_str, a.input[0],enfa_trans_func), &result_state0);
		Closure(a, transition_output(a, poped_str, a.input[1], enfa_trans_func), &result_state1);


		// Clousure1 (input is 0 ) >> 즉 큐에 없는 새로운 상태이며, 길이가 1 이상인 스트링 반환시
		if (result_state0.length() != 0) {

			res_trans_func[dfa_trans_func].src_state = poped_str;
			res_trans_func[dfa_trans_func].input = a.input[0];
			res_trans_func[dfa_trans_func++].dst_state = result_state0;

			// 즉 결과 스트링이 end state를 포함하면
			if (is_contain_end_state(result_state0, a.end_state, sizeof(a.end_state) / sizeof(string)))
				// DFA의 end state에 포함
				result_dfa.end_state[dfa_end_state++] = result_state0;

			// 큐에 없는 state라면 큐에 저장
			if (not_in_queue(queue, result_state0)) {
				push(queue, result_state0);
			}
		}


		// Clousure2 (input is 1 )   >> 즉 큐에 없는 새로운 상태이며, 길이가 1 이상인 스트링 반환시
		if (result_state1.length() != 0) {

			res_trans_func[dfa_trans_func].src_state = poped_str;
			res_trans_func[dfa_trans_func].input = a.input[1];
			res_trans_func[dfa_trans_func++].dst_state = result_state1;

			// 즉 결과 스트링이 end state를 포함하면
			if (is_contain_end_state(result_state1, a.end_state, sizeof(a.end_state) / sizeof(string)))
				// DFA의 end state에 포함
				result_dfa.end_state[dfa_end_state++] = result_state1;


			if (not_in_queue(queue, result_state1)) {
				push(queue, result_state1);
			}
		}
	}

	/*Queue가 DFA의 상태들을 저장한 것이므로 result_dfa에 저장한다.*/
	result_dfa.state = new string[tail];
	for (int r = 0; r < tail; r++) {
		result_dfa.state[r] = queue[r];
		dfa_state++;
	}

	/* res_tran_func이 DFA의 transition function을 저장한 것이므로 result_dfa에 저장한다.*/
	result_dfa.trans_f = new trans_func[dfa_trans_func];
	for (int r = 0; r < dfa_trans_func; r++) {
		result_dfa.trans_f[r].src_state = res_trans_func[r].src_state;
		result_dfa.trans_f[r].input = res_trans_func[r].input;
		result_dfa.trans_f[r].dst_state = res_trans_func[r].dst_state;
	}

	return result_dfa;
}

/* print NFA with ( state, input, transition_function, start_state, end_state ) */
void print_eNFA(Automata a) {

	cout << "\nstate :	";
	for (int i = 0; i < enfa_state ; i++)
		cout << a.state[i] << " ";

	cout << "\ninput :		" << a.input[0] << ", " << a.input[1] << endl;

	cout << "start state :	" << a.start_state << endl;

	cout << "end state :	";
	for (int t = 0; t <  max_end_state; t++)
		cout << a.end_state[t] << " ";

	cout << "\ntransition function : \n";
	for (int i = 0; i < enfa_trans_func ; i++)
		cout << "	delta (" << a.trans_f[i].src_state << "," << a.trans_f[i].input << ") = " << a.trans_f[i].dst_state << endl;



}

/* print DFA with ( state, input, transition_function, start_state, end_state ) */
void print_DFA(Automata a) {

	cout << "\nstate :	";
	for (int i = 0; i < dfa_state; i++)
		cout << a.state[i] << " ";

	cout << "\ninput :		" << a.input[0] << ", " << a.input[1] << endl;

	cout << "start state :	" << a.start_state << endl;

	cout << "end state :	";
	for (int t = 0; t < sizeof(a.end_state) / sizeof(string); t++)
		cout << a.end_state[t] << " ";

	cout << "\ntransition function : \n";
	for (int i = 0; i < dfa_trans_func; i++)
		cout << "	delta (" << a.trans_f[i].src_state << "," << a.trans_f[i].input << ") = " << a.trans_f[i].dst_state << endl;


}


/* state가 단 하나의 문자일때  output을 찾는 함수  */
string find_output(Automata a, string src, string input) {

	string dst;

	for (int r = 0; r < enfa_trans_func; r++) 
		if (a.trans_f[r].src_state == src && a.trans_f[r].input == input)
			dst = a.trans_f[r].dst_state;

	return dst;
}

/* for DFA string */
void is_str_accepted(Automata res, string input_str) {

	string state;

	state = find_output(res, res.start_state, input_str.substr(0, 1));
	
	if (state.length() == 0) {
		cout << "string is rejected....\n";
		return;
	}

	else {
		for (unsigned int t = 1; t < input_str.length() ; t++) {
			state = find_output(res, state, input_str.substr(t, 1));
			//cout << " qt : " << state << endl;
			if (state.length() == 0) {
				cout << "string is rejected....\n";
				return;
			}
		}
	}

	for (int t = 0; t < sizeof(res.end_state) / sizeof(string); t++) {
		if (state.compare(res.end_state[t]) == 0) {
			cout << "string is Accepted !!! \n ";
			return;
		}
	}

	cout << "string is rejected....\n";

}


/* for NFA string */
void check_str_in_nfa(Automata res, string input_str, string start_state) {

	string state;
	
	state = transition_output(res, start_state, input_str.substr(0, 1), nfa_trans_f);


	for (unsigned int r = 1; r < input_str.length(); r++) {
		if (state.length() == 0) {
			cout << "string is rejected....\n";
			return;
		}

		else 
			state = transition_output(res, state, input_str.substr(r, 1), nfa_trans_f);
	

		for (int t = 0; t < sizeof(res.end_state) / sizeof(string); t++) {
			if (state.find(res.end_state[t]) >= 0 && state.find(res.end_state[t]) < state.length() && res.end_state[t].length() != 0) {
				cout << "string is Accepted !!! \n ";
				return;
			}
		}

	}
	cout << "string is rejected....\n";
}

#endif