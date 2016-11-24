#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <queue>
#include <LibIV/libiv.h>

using namespace std;

#define BUTTONUPR 192
#define BUTTONUPG 134
#define BUTTONUPB 32

#define BUTTONDOWNR 255
#define BUTTONDOWNG 134
#define BUTTONDOWNB 32
#define _PI_ 3.14159

typedef pair<int,int> TPOS;
typedef pair<double,double>TPOSd;
typedef vector<TPOS> VPOS;
typedef vector<TPOSd> VPOSd;
typedef queue<TPOS> QPOS;
