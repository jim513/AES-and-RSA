#include <iostream>
#include <windows.h>
using ull = unsigned long long;
using ll = long long;
using namespace std;

ull randsize = 16384;//난수범위 지정용

ull gcd(ull a, ull b) {
	if (a < b) {
		ull temp;
		temp = a;
		a = b;
		b = temp;
	}
	while (b != 0) {
		ull c = a % b;
		a = b;
		b = c;
	}
	return a;
}

//Square and multiply
ull squareExp(ull a, ull e, ull m)
{
	ull ret = 1;
	while (e > 0) {
		if (e % 2 == 1) {
			ret *= a;
			ret %= m;
		}
		a *= a;
		a %= m;
		e /= 2;
	}
	return ret%m;
}
//확장유클리드 알고리즘
//곱셈의 역원을 구한다
ll ExtendedEuclid(ll n1,ll n2)
{
	ll T1, T2, T3, Q; //임시변수
	ll A1 = 1, A2 = 0;
	ll B1 = 0, B2 = 1, tmp = n1;

	while (n2)
	{
		Q = n1 / n2;
		
		T1 = A1 - Q * B1;
		T2 = A2 - Q * B2;
		T3 = n1 % n2;

		n1 = n2;
		n2 = T3;
		
		A1 = B1;
		A2 = B2;

		B1 = T1;
		B2 = T2;
	}

	//루프 마지막에 구해진 역원이 A2로 옮겨진다.
	if (A2 < 0)
		A2 += tmp;
	return A2;

}
//a^{(2^r)*d} = -1 (mod n)
bool millerRabin(ull n) {
	//짝수
	if (n % 2 == 0)
		return false;

	ull r = 0;
	ull d = n - 1;
	
	//random base
	ull a;
	ull result;
	while (d % 2 == 0) {
		r++;
		d /= 2;
	}
	for (int i = 0; i < 20; i++) {
		a = (rand() % n - 2) + 2;//2<= a <=n-1;
		result = squareExp(a, d, n);
		if (result == 1 || result == n - 1)
			continue;
		for (int j = 0; j < r ; j++) {
			result = squareExp(result, 2, n);
			if (result == n - 1)
				break;
			else
				return false;
		}
	}
	return true;
}

ull chineseRemain(ull ci, ull d,ull p, ull q) {
	
	ull dp = d % (p - 1);
	ull mp = squareExp(ci, dp, p);
	
	ull dq = d % (q - 1);
	ull mq = squareExp(ci, dq, q);

	ull A = (mp * q * ExtendedEuclid(p, q) + mq * p * ExtendedEuclid(q, p)) % (p * q);

	return A;
}
int main()
{
	srand( GetTickCount64());
	ull p, q ,N, phi, e, d;
	
	// 2^14 에서 2^15 -1 사이 난수 생성	
	p=(rand() % randsize) + randsize;
	q = (rand() % randsize) + randsize;
	
	//p,q 가 소수인지 확인하고 소수가 될 때까지 반복
	while (!millerRabin(p)) {
		p = (rand() % randsize) + randsize;
	}
	while (!millerRabin(q) || p==q) {
		q = (rand() % randsize) + randsize;
	}

	//p, q, N, phi 출력
	cout << "p: "<< p << endl;
	cout << "q: "<< q<< endl;
	N = p * q;
	phi = (p - 1) * (q - 1);
	cout << "N: " << N << endl;
	cout << "phi: " << phi << endl;
	
	//e,d 생성 및 출력
 	e = rand()%phi;
	while (gcd(e, phi) != 1)
		e = rand() % phi;

	d = ExtendedEuclid(phi,e);
	cout << "e :" << e << endl;
	cout << "d : " << d << endl;

	//message 입력
	ull msg, ci, de;
	cout << "Message Input = ";
	cin >> msg;
	cout << "Message = "<< msg << endl;
	
	//암호화
	cout << "**Eryption" << endl;
	ci = squareExp(msg, e, N);
	cout << "cipher = " << ci << endl;
	
	//복호화
	cout << "**Decryption" << endl;
	de = chineseRemain(ci,d, p, q);
	cout << "decrypted cipher = "<< de << endl;

	system("pause");
	return 0;
}
