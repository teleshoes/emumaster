#ifndef SN76496_H
#define SN76496_H

extern "C" {
void SN76496Write(int data);
void SN76496Update(int *buffer,int length);
int  SN76496Init(int clock,int sample_rate);
}

#endif // SN76496_H
