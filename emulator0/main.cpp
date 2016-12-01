#include <iostream>
#include <string>

using namespace std;
/*
emulator for custom 8bit pc.
simple assembly language

registers:
a,b,c,d,e,f all 8 bit
memory: 8bit pointer, 8bit val
operators:

000,mov,dst[reg,mem],scr[reg,mem,lit]
001,prt,src[reg,mem]
002,nop
003,ret,id[lit]
004,add,dst[reg,mem],src[reg,mem]
005,sub,dst[reg,mem],src[reg,mem]

operator arguments (o):
o E [0,5] o is indicating a register, o E [6,255] o is indicating a memory location, o < 0 o is indicating a literal
mem->mem is not possible;

stack is ram, you can use it.
heap is can not be modified at runtime;

flags: 0:zero,1:overflow,2:sign,3:carry

errors:
[ERROR001]: Can not move to a literal.
[ERROR002]: Invalid second argument for MOV.
[ERROR003]: Invalid first argument for MOV.
[ERROR004]: MOV can not move memory to memory.
[ERROR005]: ADD can not add to a literal.
[ERROR006]: ADD can not add from memory to memory.
[ERROR007]: SUB can not subtract from a literal.
[ERROR008]: SUB can not suntract memory from memory.
*/

char* getSource();
short getPara(char*, int&);
void sourceToOpcodes(char*, short);
void executeInstructions();
void printOpcodes(int, int, int, int, int, int);
void setFlag(bool, char);
void printFlags();
//
bool MOV(signed short, signed short);
bool PRT(signed short);
bool NOP();
bool RET(signed short);
bool ADD(signed short, signed short);
//program in ram
char opcodes[256];
signed short arg0[256];
signed short arg1[256];
//rest of the architecture
unsigned char registers[6];
unsigned char stack[250];
unsigned char heap[256];
unsigned char flags;
short IP;
bool breakFlag = false;

int main() {
	char* source = getSource();
	sourceToOpcodes(source, strlen(source));
	//printOpcodes(0,12,0,12,0,12);
	executeInstructions();

	system("PAUSE");
	return 0;
}

bool MOV(signed short a, signed short b) {
	if (a < 0) {
		cout << "[ERROR001]: Can not move to a literal." << endl;
		return false;
	}
	else if (a < 6) {//mov to reg
		if (b < 0) {//from literal
			registers[a] = (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			registers[a] = registers[b];
			return true;
		}
		else if (b) {//from mem
			registers[a] = b - 6;
			return true;
		}
		else {
			cout << "[ERROR002]: Invalid second argument for MOV." << endl;
			return false;
		}
	}
	else if (a < 256) {//mov to mem
		if (b < 0) {//from literal
			stack[a - 6] = (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			stack[a - 6] = registers[b];
			return true;
		}
		else if (b) {//from mem
			cout << "[ERROR004]: MOV can not move memory to memory" << endl;
			return false;
		}
		else {
			cout << "[ERROR002]: Invalid second argument for MOV." << endl;
			return false;
		}
	}
	else {
		cout << "[ERROR003]: Invalid first argument for MOV." << endl;
		return false;
	}
}
bool PRT(signed short a) {
	if (a < 6) {//print reg
		cout << (short)registers[a] << endl;
	}
	else {//print mem
		cout << (short)stack[a - 6] << endl;
	}
	return true;
}
bool NOP() {
	return true;//:D
}
bool RET(signed short a) {
	breakFlag = true;
	cout << "[PROGRAM]: The program has stopped with exit-code " << -a - 1 << "." << endl;
	return true;
}
bool ADD(signed short a, signed short b) {
	if (a < 0) {
		cout << "[ERROR005]: ADD can not add to a literal." << endl;
		return false;
	}
	else if (a < 6) {//add to reg
		if (b < 0) {
			short ans = registers[a] + (-b) - 1;
			if (ans > 255)
				setFlag(true, 1);
			registers[a] += (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			short ans = registers[a] + registers[b];
			if (ans > 255)
				setFlag(true, 1);
			registers[a] += registers[b];
			return true;
		}
		else {//from mem
			short ans = registers[a] + stack[b - 6];
			if (ans > 255)
				setFlag(true, 1);
			registers[a] += stack[b - 6];
			return true;
		}
	}
	else {//add to mem
		if (b < 0) {
			short ans = stack[a - 6] + (-b) - 1;
			if (ans > 255)
				setFlag(true, 1);
			stack[a - 6] += (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			short ans = stack[a - 6] + registers[b];
			if(ans > 255)
				setFlag(true, 1);
			stack[a - 6] += registers[b];
			return true;
		}
		else {//from mem
			cout << "[ERROR006]: ADD can not add from memory to memory." << endl;
			return false;
		}
	}
}
bool SUB(signed short a, signed short b) {
	if (a < 0) {
		cout << "[ERROR007]: SUB can not subtract from a literal." << endl;
		return false;
	}
	else if (a < 6) {//sub to reg
		if (b < 0) {
			short ans = registers[a] - ((-b) - 1);
			if (ans < 0)
				setFlag(true, 1);
			registers[a] -= (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			short ans = registers[a] - registers[b];
			if (ans < 0)
				setFlag(true, 1);
			registers[a] -= registers[b];
			return true;
		}
		else {//from mem
			short ans = registers[a] - stack[b - 6];
			if (ans < 0)
				setFlag(true, 1);
			registers[a] -= stack[b - 6];
			return true;
		}
	}
	else {//add to mem
		if (b < 0) {
			short ans = stack[a - 6] - (-b) - 1;
			if (ans < 0)
				setFlag(true, 1);
			stack[a - 6] -= (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			short ans = stack[a - 6] - registers[b];
			if (ans < 0)
				setFlag(true, 1);
			stack[a - 6] -= registers[b];
			return true;
		}
		else {//from mem
			cout << "[ERROR008]: SUB can not suntract memory from memory." << endl;
			return false;
		}
	}
}

void setFlag(bool val, char pos) {
	if (val) {//set flag to 1
		flags |= 1 << pos;
	}
	else {//clear flag to 0
		flags &= ~(1 << pos);
	}
}
void printFlags() {
	cout << "[SYSTEM][FLAGS]:";
	for (int i = 0; i < 8; i++) {
		int bit = (flags >> i) & 1;
		cout << bit;
	}
	cout << endl;
}

void executeInstructions() {
	for (IP = 0; IP < 256; IP++) {
		for (int i = 0; i < 8; i++)
		{
			setFlag(false, i);
		}
		bool f = true;
		switch (opcodes[IP]) {
		case 0: {
			f = MOV(arg0[IP], arg1[IP]);
			break;
		}
		case 1: {
			f = PRT(arg0[IP]);
			break;
		}
		case 2: {
			f = NOP();
			break;
		}
		case 3: {
			f = RET(arg0[IP]);
			break;
		}
		case 4: {
			f = ADD(arg0[IP], arg1[IP]);
			break;
		}
		case 5: {
			f = SUB(arg0[IP], arg1[IP]);
			break;
		}
		}
		printFlags();
		if (!f) {
			cout << "[@line]: " << IP << endl;
			cout << "[SYSTEM]: program terminated." << endl;
		}
		if (breakFlag) {
			break;
		}
	}
}

void sourceToOpcodes(char* s, short len) {
	char command[4];
	char opI = 0, a0I = 0, a1I = 0;

	int i = 0;
	while (i < len) {
		//capture instruction
		command[0] = s[i + 0];
		command[1] = s[i + 1];
		command[2] = s[i + 2];
		command[3] = '\0';

		if (!strcmp(command, "mov"))
			opcodes[opI] = 0;
		else if (!strcmp(command, "prt"))
			opcodes[opI] = 1;
		else if (!strcmp(command, "nop"))
			opcodes[opI] = 2;
		else if (!strcmp(command, "ret"))
			opcodes[opI] = 3;
		else if (!strcmp(command, "add"))
			opcodes[opI] = 4;
		else if (!strcmp(command, "sub"))
			opcodes[opI] = 5;
		i += 3;//3 chars, example: "mov"
		opI++;

		//end of line(;) or first argument(,)
		if (s[i] == ',') {
			i++;
			short para = getPara(s, i);
			arg0[a0I] = para;
			a0I++;
		}
		else {
			a0I += 1;
		}

		//end of line(;) or next argument(,)
		if (s[i] == ',') {
			i++;
			short para = getPara(s, i);
			arg1[a1I] = para;
			a1I++;
			i++;
		}
		else {
			a1I++;
			i++;
		}
	}
}

void printOpcodes(int insMin, int insMax, int a0Min, int a0Max, int a1Min, int a1Max) {
	for (int i = insMin; i < insMax + 1; i++) {
		cout << "[SYSTEM::OPCODES::" << i << "]: " << (short)opcodes[i] << endl;
	}
	for (int i = a0Min; i < a0Max + 1; i++) {
		cout << "[SYSTEM::ARG0::" << i << "]: " << arg0[i] << endl;
	}
	for (int i = a1Min; i < a1Max + 1; i++) {
		cout << "[SYSTEM::ARG1::" << i << "]: " << arg1[i] << endl;
	}
}

short getPara(char* s, int& i) {
	//capture argument: 0-5 = register, 5 - 255 = memorylocation, < 0 is -(decimal)
	if (s[i] == '*') {
		char numSTR[3];
		numSTR[0] = s[i + 1];
		numSTR[1] = s[i + 2];
		numSTR[2] = s[i + 3];
		i += 4;//4 chars example: "*002"
		return (short)(stoi(numSTR) + 6);
	}
	else if (s[i] == '#') {
		char numSTR[3];
		numSTR[0] = s[i + 1];
		numSTR[1] = s[i + 2];
		numSTR[2] = s[i + 3];
		i += 4;//4 chars example: "#066"
		return (short)((-stoi(numSTR))-1);
	}
	else {
		short answ = s[i] - 'a';
		i += 1;//1 char example: "a"
		return answ;
	}
}

char* getSource() {
	return
		"mov,a,#000;"
		"prt,a;"
		"add,a,#155;"
		"prt,a;"
		"sub,a,#055;"
		"prt,a;"
		"sub,a,#120;"
		"prt,a;"
		"ret,#000;"
		;
}
/*
"mov,a,#003;"
"mov,b,#004;"
"add,a,b;"
"prt,a;"
"prt,b;"
"mov,*222,a;"
"mov,c,#056;"
"prt,c;"
"prt,*222;"
"nop;"
"mov,d,#022;"
"mov,*015,d;"
"prt,*015;"
"ret,#000;"
;
*/