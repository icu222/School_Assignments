
#include <iostream>
#include <string>
using namespace std;

int get_Expr_idx(string); // 식에 . 없으면 -1 리턴
int get_endExpr_idx(string,int); // 
int get_Expr_len(int,int); //
int get_cutExpr_idx(string, int, int);
void SKI_translater(string&);
void translate_S(string&, int, int);
void translate_K(string&, int);
void translate_I(string&, int);
int Q_delete_paren(string);
void beta_conversion(string&);


int main() {

	string lambda_expr;
	
	cout << " ====== Lamba expression to SKI expression ======\n\n";
	cout << " 원래 식 : /f./x.f(fx) \n";
	cout << " 표현 법 : Lf.Lx.f(fx) \n";
	cout << "\n 원래 식 : (/m n f x -> m f (n f x)) (/f x -> f x) (/f x -> f x) \n";
	cout << " 표현 법 : (Lm.Ln.Lf.Lx.mf(nfx))(Lf.Lx.fx)(Lf.Lx.fx)\n";

	cout << "\n\n 람다 식을 입력해주세요 : ";
	cin >> lambda_expr;

	///*
	if(lambda_expr[0] == '(')
		beta_conversion(lambda_expr);
	
	SKI_translater(lambda_expr);

	cout << "\n\n RESULT : \n\n";
	cout << lambda_expr << endl;
	//*/
	system("pause");
	return 0;
}

//원리 : L, 즉 람다 다음에는 변수가 오므로, 제일 왼쪽의 L을 찾아 그 위치에서 +1을 하면 
// 가장 왼쪽의 변수의 위치를 찾을 수 있다. 
int find_lesftmost_lambda_var(string s) {

	for (unsigned int l_index = 0;  l_index < s.length(); l_index++)
		if (s[l_index] == 'L')
			return  l_index + 1;

	return -1;
}

// 람다 식에서 '.' 이후의 식 중  변수가 나타나는 모든 위치를 스트링에 저장
// ex : Lx.yyy(x)yxx >> L의 인덱스가 0이라고 할때, 7,10,11 을 스트링에 저장
void find_var_inExpr(string s, int var_idx, int dotpos, int endpos , int* var_index, int* num) {

	int k = 0;

	for (int t = dotpos + 1; t <= endpos; t++) {
		// Lx.Ly.yyx 에서 L'x' == yy'x'일때
		if (s[t] ==s[var_idx]) {
			var_index[k++] = t;
		}
	}
	*num = k;
}

// 베타 컨버젼 이전에 원래 식의 '.' 위치를 찾는다.
int beta_find_dot_idx(string s, int endpos) {

	for (int t = endpos; t >= 0; t--) {
		if (s[t] == '.')
			return t;
	}

	return -1;
}

// 베타 컨버젼 이전에 원래 식의 끝 위치를 찾는다. 
// (Lx.fx)(Ld.d) => 6 즉 ')'의 인덱스 리턴
int beta_find_end_idx(string s) {

	int pair_paren = 0;

	for (unsigned int y = 0; y < s.length(); y++) {
		if (s[y] == '(') 
			pair_paren++;
		
		if (s[y] == ')') {
			pair_paren--;
			if (pair_paren == 0)
				return y;
		}
	}
	return -1;
}

// beta conversion  이 적용되는 베타 식의 끝에 대한 index를 구한다. ')'의 index return 
int beta_find_beta_endpos(string s, int endpos) {
	
	// (Lx.f)(Lx.x) >> endpos: 4 (f의 위치) + 2 = beta의 'L'의 위치
	int pair_paren = 0;

	for (unsigned int t = endpos + 1; t < s.length(); t++) {
		if (s[t] == '(')
			pair_paren++;

		if (s[t] == ')') {
			pair_paren--;
			if (pair_paren == 0) {
				return t;
			}
		}

	}

	// 적용 할 beta식이 없을 때 람다 식의 맨 끝 index return 
	return s.length() - 1;
}



void beta_conversion(string& s) {
	/*
	1. 람다 변수를 찾는다. '  var_idx
	2. 람다 식의 끝을 찾는다.  endpos
	3. 람다 식의 '.'을 찾는다. dotpos
	4. 식의 람다 변수 위치를 찾는다.  find_var_inEXPR
	5. 변형 시작
	6. beta conversion 이후 양쪽의 ()를 없애준다. 
	*/
	int var_idx = find_lesftmost_lambda_var(s);
	int endpos = beta_find_end_idx(s);
	int dotpos = beta_find_dot_idx(s, endpos);
	int endbeta_pos = beta_find_beta_endpos(s, endpos);
	int num_of_var = 0;

	string beta;
	int var_loc[10];

	// beta conversion 시작
	// 더 이상 beta conversion 할 식이 없으면 종료
	while (endpos != s.length()-1) {

		// 람다 식의 beta conversion을 적용할 변수들의 위치를 찾는다. 
		find_var_inExpr(s, var_idx, dotpos, endpos, var_loc, &num_of_var);
	


		// beta 식을 자른다. (베타 식) 전체 저장
		// ex) (Lm.Ln.Lf.Lx.mf(nfx))(Lf.Lx.fx)(Lf.Lx.fx) >> (Lf.Lx.fx) 저장
		beta = s.substr(endpos + 1, endbeta_pos - endpos );
		


		// 전체 식에서 베타 식을 제거
		//ex) (Lm.Ln.Lf.Lx.mf(nfx))(Lf.Lx.fx)(Lf.Lx.fx) >> (Lm.Ln.Lf.Lx.mf(nfx))(Lf.Lx.fx)
		s.erase(endpos + 1, endbeta_pos - endpos );




		// beta conversion이 적용되는 람다 식의 변수의 위치에 beta 삽입
		// ex) (Lm.Ln.Lf.Lx.mf(nfx))(Lf.Lx.fx) >> (Lm.Ln.Lf.Lx.(Lf.Lx.fx)f(nfx))(Lf.Lx.fx)
		for (int r = 0; r < num_of_var; r++) {
			s.erase(var_loc[r], 1);
			s.insert(var_loc[r], beta);
			
		}

		// 람다 변수 전체 식에서 제거
		//(Lm.Ln.Lf.Lx.(Lf.Lx.fx)f(nfx))(Lf.Lx.fx) >> (Ln.Lf.Lx.(Lf.Lx.fx)f(nfx))(Lf.Lx.fx)
		s.erase(1, 3);

		var_idx = find_lesftmost_lambda_var(s);
		endpos = beta_find_end_idx(s);
		dotpos = beta_find_dot_idx(s, endpos);
		endbeta_pos = beta_find_beta_endpos(s, endpos);
		num_of_var = 0;

	}
	

	// 이제 양 쪽 ()를 제거해야 된다. 
	// ex) (Lf.Lx.(Lf.Lx.fx)f((Lf.Lx.fx)fx)) >> Lf.Lx.(Lf.Lx.fx)f((Lf.Lx.fx)fx) 
	s.erase(s.length() - 2, 1);
	s.erase(0, 1);

}




// 오른쪽-> 왼쪽으로 처음 나타나는 람다 식의 '.' 위치 반환
int get_Expr_idx(string s) {

	// 식의 오른쪽에서부터 '.'의 위치를 찾는다. ( 가장 먼저 연산해야되는 식을 찾는 과정 )
	for (int idx = s.length() - 1; idx >= 0 ; idx--) 
		if (s[idx] == '.')
			return idx;

	// 만약 .이 없다면 ( 모든 식이 SKI로 연산되었거나 .을 쓰지 않은 경우(에러) )
	return -1;

}

// 가장 먼저 연산하는 람다 식의 끝 index를 반환
int get_endExpr_idx(string s, int dotpos){

	//int end_idx = 0;
	unsigned int t = dotpos;
	int paren = 0;

	while(t < s.length()){

		if (s[t] == '(') {
			paren += 1;

		}

		if (s[t] == ')') {
			paren -= 1;
			if (paren == -1)
				return t-1;
		}
		
		t++;
	}

	return  t-1;
}

// '.' 이후의 람다 식의 길이 반환
int get_Expr_len(int start, int end){

	return (end - start);
}

// '.'이후의 식을 반으로 쪼개는데, 쪼갠 것들 중 앞 식의 마지막 idx 반환 
/*  예시
 Lx.(aa)(bb) , '.' = 2  ---> return 7
 Lx.aaaaaaaa , '.' = 2  ---> return 10
 Lx.(aaa(a)a(a))aa, '.' = 2  ---> return 15
 Lx.aaa(a)a(a)aa, '.' = 2  ---> return 13
 Lx.aaaaa(bb) , '.' = 2  ---> return 8
 Lx.(aaaaads)as(dsa(bb)),   '.' = 2  --->  14 반환 
 Lx.(aaaaads)as(d(s)aa(bb)), '.' = 2  --->   14 반환  
*/
int get_cutExpr_idx(string s, int dotpos, int endpos) {

	int pair_paren = 0;

	// 맨 뒤가 )로 끝난다면 : 이 )에 묶이는 (의 위치를 찾는다. 도중에 나오는 ()는 무시하는 방법
	// ex: Lx.dsadasd(fdfdfd(dsfsdfds(fdsfsd)sdfsdf)dsfdsfsd) >> Lx.dsadasd <-요 다음 자리 반환
	if (s[endpos] == ')'){

		pair_paren++;

		for (int i = endpos - 1; i >= dotpos; i--) {
			if (s[i] == '(') {
				pair_paren--;
				
				if(pair_paren ==0)
					// 즉 모든 () 쌍이 만들어지면 >> 이때 '('는 잘리는 식의 가장 바깥의 괄호다. 
					//  == 처음 if문의 ')'에 대응되는 '(' 
					return i;
			}
			if(s[i] == ')')
				pair_paren++;
		}
	}

	// 맨 뒤가 )로 끝나지 않는다. >> )가 나올때까지 반복
	//ex: Lx.dsadas(fdsfsdf)fdsfsd >>   dsadas(fdsfsdf)   fdsfsd    >> 다음 자리 index 반환
	else {
		for (int i = endpos - 1; i >= dotpos; i--) {
			if (s[i] == ')') {
				return i+1;
			}
		}
		return endpos;
	}
	return - 1;
}


int Q_delete_paren( string s) {
	
	int do_delete = 1;
	int rparen = 0;
	int check_error = 0;

	if (s[0] == '(' && s[s.length() - 1] == ')') {
		
		for (unsigned i = 1; i < s.length() - 1; i++) {
			if (s[i] == ')')
				check_error++;

			if (s[i] == '(')
				check_error--;
		}

		// 즉 맨 앞,뒤의 ()를 제거해도 ()의 개수가 일치할 때 
		//  (Kf)(S(Kf)I) 는 양쪽 제거해도 Kf)(S(Kf)I 라서 실제로는 에러임. 이걸 잡아야됨
		if (check_error == 0) {
			
			for (unsigned i = 1; i < s.length() - 1; i++) {
				if (s[i] == ')') {
					do_delete--;
					if (do_delete == 0) {
						
						break;
					}
				}
				if(s[i] == '(')
					do_delete++;
			}
		}

		// 맨 앞,뒤의 ()를 제거하면 ()의 개수가 일치하지 않을 때
		// (Kf)aaa >> Kf)aaa (ERROR)
		else {
			
			do_delete = 0;
		}
	}

	else {
		
		do_delete = 0;
	}

	
	return do_delete;

}


void SKI_translater(string& expr) {

	int dot_pos = 0;
	int end_expr_pos = 0;
	int expr_len = 0;
	int cut_pos = 0;

	// 즉 input으로 받은 스트링에 '.'이 없어질 때 까지 반복
	// get_Expr_idx은 스트링에 '.'가 없으면 -1을 반환하므로
	while (get_Expr_idx(expr) != -1) {
		dot_pos = get_Expr_idx(expr);
		end_expr_pos = get_endExpr_idx(expr, dot_pos);

		// 한 람다 식의 길이가 1 일때 >> K or I combinator
		if (get_Expr_len(dot_pos, end_expr_pos) == 1) {
			// Lx.x 인 경우
			if (expr[dot_pos - 1] == expr[end_expr_pos])
				translate_I(expr, dot_pos);
			// Lx.y 인 경우
			else
				translate_K(expr, dot_pos);
		}

		// 한 람다 식의 길이가 2 이상일때 >> S combinator
		else
			translate_S(expr, dot_pos, end_expr_pos);


	}


}


// 한 람다 식 >> S(람다 식)(람다 식)
// ...Lx.statment.. >> ...S(Lx.state a)(Lx.state b)....
void translate_S(string& s, int dotpos, int endpos) {

	int do_delete_paren = 0; // 잘린 식의 괄호를 제거해야하는지 판단하는 변수
	// ex. Lx.(fx)(fx) >> S(Lx.fx)(Lx.fx) 로 바뀌기 위해 (fx) -> fx 즉 괄호가 없어져야됨.

	int cut_pos = get_cutExpr_idx(s, dotpos, endpos); // 식을 자르는 위치를 저장

	string lambda_variable = s.substr(dotpos - 1, 1); // 람다 변수 (L다음에 오는 알파벳) 를 찾아서 저장

	string rest_statement = s.substr(endpos + 1, s.length() - 1);
	// S transform에 참여하지 않는 람다 식들을 저장

	string div_statement1 = "L";
	string div_statement2 = "L";
	string tmp_expr;


	tmp_expr = s.substr(dotpos + 1, (cut_pos - dotpos -1));
	
	
	do_delete_paren = Q_delete_paren(tmp_expr);

	// 잘린 식에서 ()를 삭제해야 되는지 판단
	// 만약 ()를 삭제 안하면 Lx.(fx)(fx) >> S(Lx.(fx))(Lx.(fx)) 이렇게 ()가 불필요하게 많이 쓰이고 연산에도 지장
	if (do_delete_paren) {
		tmp_expr = tmp_expr.substr(1, tmp_expr.length() - 2);
	}

	// S(요 식을 만듦.)(Lx.xxxx)
	// div1 =  ( + L + x + . + <expr> )
	div_statement1 = "(" + div_statement1 + lambda_variable + "." + tmp_expr + ")";
	

	tmp_expr.clear();
	
	
	// 위와 동일하게 반복  S(Lx.xxxx)(이 식을 만드는 과정)
	tmp_expr = s.substr(cut_pos, (endpos- cut_pos + 1));
	

	do_delete_paren = Q_delete_paren(tmp_expr);

	if (do_delete_paren) {
		tmp_expr = tmp_expr.substr(1, tmp_expr.length() - 2);
	}

	// S(Lx.xxxx)(요 식을 만듦.)
	// div2 =  ( + L + x + . + <expr> )
	div_statement2 = "(" + div_statement2 + lambda_variable + "." + tmp_expr + ")";
	

	// 3. s에서 원래의 람다 식을 제거
	s.erase(dotpos - 2, s.length());

	// 최종 : 잘린 전체 식(s) + 나눈 식1 + 나눈 식2 + lump
	// ex ) ......  +  S  + (Lx.xxx) + (Lx.yyy) + .....
	s = s + "S" + div_statement1 + div_statement2 + rest_statement;

	
}


// ...Lx.y... >> ...Ky...
void translate_K(string& s, int dotpos ){

	string lump;
	string ch = s.substr(dotpos + 1, 1);
	// 람다 식의 문자를 저장
	// ex) Ly.f >> f 저장
	


	// Lx.x이후의 ... 식을 복사
	lump = s.substr(dotpos + 2, s.length() - 1);

	// s에서 ...Lx.x...의 Lx.x...를 전부 삭제
	s.erase(dotpos - 2, s.length());

	// K식 추가 이거 내용 바꿔
	s = s + "K" + ch + lump;

}


// .....Lx.x.... >> .....I... 
void translate_I(string& s, int dotpos) {
	string lump;

	// Lx.x이후의 ... 식을 복사
	lump = s.substr(dotpos + 2, s.length() - 1);

	// s에서 ...Lx.x...의 Lx.x...를 전부 삭제
	s.erase(dotpos - 2, s.length());

	s = s + "I" + lump;

	// (I) --> I 현재 I는 s[dotpos-2] 에 위치
	if (s[dotpos - 3] == '(' && s[dotpos - 1] == ')')
		s = s.substr(0, (dotpos - 3)) + s.substr(dotpos - 2, 1) + s.substr(dotpos, (s.length() - dotpos));

}
