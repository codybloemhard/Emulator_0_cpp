#include <iostream>
#include <string>

using namespace std;
/*
emulator for custom 8bit pc.
simple assembly language
*/
char* getSource();
short getPara(char*, int&);
void sourceToOpcodes(char*, short);
void executeInstructions();
void printOpcodes(int, int, int, int, int, int);
void setFlag(bool, char);
bool getFlag(char);
void printFlags();
void clearFlags();
////
bool MOV(signed short, signed short);
bool PRT(signed short);
bool NOP();
bool RET(signed short);
bool ADD(signed short, signed short);
bool SUB(signed short, signed short);
bool GTO(signed short);
bool CMP(signed short, signed short);
bool JJZ(signed short);
bool JNZ(signed short);
bool JJS(signed short);
bool JJG(signed short);
bool JZS(signed short);
bool JZG(signed short);
bool MUL(signed short);
bool DIV(signed short);
bool GCH(signed short);
bool PSH(signed short);
bool POP(signed short);
bool CAL(signed short);
//program in ram
char opcodes[256];
signed short arg0[256];
signed short arg1[256];
//rest of the architecture
unsigned char registers[6];
unsigned char stack[16];
unsigned char ram[250];
unsigned char heap[256];
unsigned char flags;
unsigned short IP;
unsigned short SP;

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
	clearFlags();
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
			registers[a] = ram[b - 6];
			return true;
		}
		else {
			cout << "[ERROR002]: Invalid second argument for MOV." << endl;
			return false;
		}
	}
	else if (a < 256) {//mov to mem
		if (b < 0) {//from literal
			ram[a - 6] = (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			ram[a - 6] = registers[b];
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
	clearFlags();
	if (a < 0) {
		cout << (short)(-a) - 1 << endl;
	}
	else if (a < 6) {//print reg
		cout << (short)registers[a] << endl;
	}
	else {//print mem
		cout << (short)ram[a - 6] << endl;
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
	clearFlags();
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
			short ans = registers[a] + ram[b - 6];
			if (ans > 255)
				setFlag(true, 1);
			registers[a] += ram[b - 6];
			return true;
		}
	}
	else {//add to mem
		if (b < 0) {
			short ans = ram[a - 6] + (-b) - 1;
			if (ans > 255)
				setFlag(true, 1);
			ram[a - 6] += (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			short ans = ram[a - 6] + registers[b];
			if(ans > 255)
				setFlag(true, 1);
			ram[a - 6] += registers[b];
			return true;
		}
		else {//from mem
			cout << "[ERROR006]: ADD can not add from memory to memory." << endl;
			return false;
		}
	}
}
bool SUB(signed short a, signed short b) {
	clearFlags();
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
			short ans = registers[a] - ram[b - 6];
			if (ans < 0)
				setFlag(true, 1);
			registers[a] -= ram[b - 6];
			return true;
		}
	}
	else {//add to mem
		if (b < 0) {
			short ans = ram[a - 6] - (-b) - 1;
			if (ans < 0)
				setFlag(true, 1);
			ram[a - 6] -= (-b) - 1;
			return true;
		}
		else if (b < 6) {//from reg
			short ans = ram[a - 6] - registers[b];
			if (ans < 0)
				setFlag(true, 1);
			ram[a - 6] -= registers[b];
			return true;
		}
		else {//from mem
			cout << "[ERROR008]: SUB can not suntract memory from memory." << endl;
			return false;
		}
	}
}
bool GTO(signed short a) {
	if (a < 0 && a > -256)
		IP = (-a) - 1 - 1;// ((-a) - 1) gets the line to jump to, -1 cause the exec-forloop does ip++ after this call so this corrects this.
	else {
		cout << "[ERROR009]: GTO is used with an invalid adress." << endl;
		return false;
	}
	return true;
}
bool CMP(signed short a, signed short b) {
	clearFlags();
	signed short ans = 0;
	if (a < 0) {
		cout << "[ERROR013]: CMP can not take a literal as first argument." << endl;
		return false;
	}
	else if (a < 6) {//cmp reg to
		if (b < 0) {//literal
			ans = registers[a] - ((-b) - 1);
		}
		else if (b < 6) {//reg
			ans = registers[a] = registers[b];
		}
		else if (b) {//mem
			ans = registers[a] - (b - 6);
		}
		else {
			cout << "[ERROR010]: Invalid second argument for CMP." << endl;
			return false;
		}
	}
	else if (a < 256) {//cmp mem to
		cout << "[ERROR011]: CMP can not take memory as first argument." << endl;
		return false;
	}
	else {
		cout << "[ERROR012]: Invalid first argument for CMP." << endl;
		return false;
	}
	if (ans == 0)
		setFlag(true, 0);
	else if (ans < 0)
		setFlag(true, 2);
	else
		setFlag(false, 2);

	return true;
}
bool JJZ(signed short a) {
	if (getFlag(0)) {
		bool jumped = GTO(a);
		if (!jumped)
			cout << "[ERROR014]: JJZ failed to jump." << endl;
	}
	return true;
}
bool JNZ(signed short a) {
	if (!getFlag(0)) {
		bool jumped = GTO(a);
		if (!jumped)
			cout << "[ERROR015]: JNZ failed to jump." << endl;
	}
	return true;
}
bool JJS(signed short a) {
	if (getFlag(2) && !getFlag(0)) {
		bool jumped = GTO(a);
		if (!jumped)
			cout << "[ERROR016]: JJS failed to jump." << endl;
	}
	return true;
}
bool JJG(signed short a) {
	if (!getFlag(2) && !getFlag(0)) {
		bool jumped = GTO(a);
		if (!jumped)
			cout << "[ERROR017]: JJG failed to jump." << endl;
	}
	return true;
}
bool JZS(signed short a) {
	if (getFlag(0) || getFlag(2)) {
		bool jumped = GTO(a);
		if (!jumped)
			cout << "[ERROR018]: JZS failed to jump." << endl;
	}
	return true;
}
bool JZG(signed short a) {
	if (getFlag(0) || !getFlag(2)) {
		bool jumped = GTO(a);
		if (!jumped)
			cout << "[ERROR019]: JZG failed to jump." << endl;
	}
	return true;
}
bool MUL(signed short a) {
	clearFlags();
	if (a < 0) {
		short ans = registers[0] * ((-a) - 1);
		if (ans > 255)
			setFlag(true, 1);
		registers[0] *= (-a) - 1;
		return true;
	}
	else if (a < 6) {//from reg
		short ans = registers[0] * registers[a];
		if (ans > 255)
			setFlag(true, 1);
		registers[0] *= registers[a];
		return true;
	}
	else {//from mem
		short ans = registers[0] * ram[a - 6];
		if (ans > 255)
			setFlag(true, 1);
		registers[0] *= ram[a - 6];
		return true;
	}
}
bool DIV(signed short a) {
	clearFlags();
	if (a < 0) {
		short ans = registers[0] / ((-a) - 1);
		short rem = registers[0] % ((-a) - 1);
		registers[0] = ans;
		registers[3] = rem;
		return true;
	}
	else if (a < 6) {//from reg
		short ans = registers[0] / registers[a];
		short rem = registers[0] % registers[a];
		registers[0] = ans;
		registers[3] = rem;
		return true;
	}
	else {//from mem
		short ans = registers[0] / ram[a - 6];
		short rem = registers[0] % ram[a - 6];
		registers[0] = ans;
		registers[3] = rem;
		return true;
	}
}
bool GCH(signed short a) {
	if (a < 256) {
		cin >> ram[a - 6];
		return true;
	}
	else {
		cout << "[ERROR020]: GIN received a faulty memory adress." << endl;
		return false;
	}
}
bool PSH(signed short a) {
	if (SP >= 16) {
		cout << "[ERROR021]: Stackoverflow." << endl;
		return false;
	}
	else {
		if (a < 0)
			stack[SP++] = -a - 1;
		else if (a < 6)
			stack[SP++] = registers[a];
		else if (a < 255)
			cout << "[ERROR022]: PSH cannot push from ram." << endl;
		else {
			cout << "[ERROR023]: PSH received a nonvalid argument." << endl;
			return false;
		}
		return true;
	}
}
bool POP(signed short a) {
	if (SP >= 0) {
		if (a < 0) {
			cout << "[ERROR024]: POP cannot pop into a literal." << endl;
			return false;
		}
		else if (a < 6)
			registers[a] = stack[SP - 1];
		else if (a < 255) {
			cout << "[ERROR025]: POP cannot pop into ram." << endl;
			return false;
		}
		else {
			cout << "[ERROR026]: POP received a nonvalid argument." << endl;
			return false;
		}
		stack[SP--] = 0;
		return true;
	}
	return false;
}
bool CAL(signed short) {
	return false;
}

void setFlag(bool val, char pos) {
	if (val) {//set flag to 1
		flags |= 1 << pos;
	}
	else {//clear flag to 0
		flags &= ~(1 << pos);
	}
}
bool getFlag(char pos) {
	int bit = (flags >> pos) & 1;
	if (bit == 1)
		return true;
	return false;
}
void printFlags() {
	cout << "[SYSTEM][FLAGS]:";
	for (int i = 0; i < 8; i++) {
		int bit = (flags >> i) & 1;
		cout << bit;
	}
	cout << endl;
}
void clearFlags() {
	flags ^= flags;
}

void executeInstructions() {
	int i = 0;
	for (IP = 0; IP < 256; IP++) {
		i++;
		if (i > 100000) {
			cout << "[SYSTEM]: Exited because the program took more than 100000 steps." << endl;
			break;
		}
		bool f = true;
		switch (opcodes[IP]) {
		case   0: { f = MOV(arg0[IP], arg1[IP]);	break; }
		case   1: { f = PRT(arg0[IP]);				break; }
		case   2: { f = NOP();						break; }
		case   3: { f = RET(arg0[IP]);				break; }
		case   4: { f = ADD(arg0[IP], arg1[IP]);    break; }
		case   5: { f = SUB(arg0[IP], arg1[IP]);	break; }
		case   6: { f = GTO(arg0[IP]);				break; }
		case   7: { f = CMP(arg0[IP], arg1[IP]);    break; }
		case   8: { f = JJZ(arg0[IP]);				break; }
		case   9: { f = JNZ(arg0[IP]);              break; }
		case  10: { f = JJS(arg0[IP]);              break; }
		case  11: { f = JJG(arg0[IP]);              break; }
		case  12: { f = JZS(arg0[IP]);              break; }
		case  13: { f = JZG(arg0[IP]);              break; }
		case  14: { f = MUL(arg0[IP]);				break; }
		case  15: { f = DIV(arg0[IP]);				break; }
		case  16: { f = GCH(arg0[IP]);				break; }
		case  17: { f = PSH(arg0[IP]);				break; }
		case  18: { f = POP(arg0[IP]);				break; }
		case  19: { f = CAL(arg0[IP]);				break; }
		}
		//printFlags();
		if (!f) {
			cout << "[@line]: " << IP << endl;
			cout << "[SYSTEM]: program terminated." << endl;
			breakFlag = true;
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

		if (!strcmp(command, "mov"))		opcodes[opI] = 0;
		else if (!strcmp(command, "prt"))	opcodes[opI] = 1;
		else if (!strcmp(command, "nop"))	opcodes[opI] = 2;
		else if (!strcmp(command, "ret"))	opcodes[opI] = 3;
		else if (!strcmp(command, "add"))	opcodes[opI] = 4;
		else if (!strcmp(command, "sub"))	opcodes[opI] = 5;
		else if (!strcmp(command, "gto"))	opcodes[opI] = 6;
		else if (!strcmp(command, "cmp"))	opcodes[opI] = 7;
		else if (!strcmp(command, "jjz"))	opcodes[opI] = 8;
		else if (!strcmp(command, "jnz"))	opcodes[opI] = 9;
		else if (!strcmp(command, "jjs"))	opcodes[opI] = 10;
		else if (!strcmp(command, "jjg"))	opcodes[opI] = 11;
		else if (!strcmp(command, "jzs"))	opcodes[opI] = 12;
		else if (!strcmp(command, "jzg"))	opcodes[opI] = 13;
		else if (!strcmp(command, "mul"))	opcodes[opI] = 14;
		else if (!strcmp(command, "div"))	opcodes[opI] = 15;
		else if (!strcmp(command, "gch"))	opcodes[opI] = 16;
		else if (!strcmp(command, "psh"))	opcodes[opI] = 17;
		else if (!strcmp(command, "pop"))	opcodes[opI] = 18;
		else if (!strcmp(command, "cal"))	opcodes[opI] = 19;

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
		"ret,#000;"
		;
}