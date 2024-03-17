#pragma once


struct greeting {
	long long count;
	double f;
	char name[50];
};

void greet(struct greeting* g);   // API
