#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include<assert.h>
#include<algorithm>


//HP��str�Ɏg�p����ėp����
//�o�b�t�@��f�o�t�ɑΉ����邽�߃f�t�H���g�l���ۑ����Ă���
//
//�g�p��
//  RangeValue<float> hp(0, 100);
//  �񕜂�����ꍇ�� hp.setMax();
//  RangeValue<float> str( 10*0.5, 10*1.5, 10 );
//  str.mul(2);
//  str.setToDefault();
//  
template< class T >
class RangeNumber {
public:
	RangeNumber()
	{}

	//default�l��max�̒l��
	RangeNumber(
			const T &minVal,
			const T &maxVal)
		:mDefault(maxVal),
		mCurrent(maxVal),
		mMin(minVal),
		mMax(maxVal)
	{
		assert(mMax > mMin );
	}
	
	RangeNumber(
			const T &minVal,
			const T &maxVal,
			const T &defaultVal )
		:mDefault(defaultVal),
		mCurrent(defaultVal),
		mMin(minVal),
		mMax(maxVal)
	{
		assert(mMax > mMin );
	}

	void init(
		const T &minVal,
		const T &maxVal,
		const T &defaultVal)
	{
		mDefault = defaultVal;
		mCurrent = defaultVal;
		mMin = minVal;
		mMax = maxVal;

		assert(mMax > mMin);
	}
	
	~RangeNumber(){}
	
	void add(const T& n){
		mCurrent += n;
		//mCurrent = std::clamp( mCurrent, mMin, mMax );
		mCurrent = clamp( mCurrent, mMin, mMax );
	}
	
	void mul( const float &n){
		mCurrent *= n;
		//mCurrent = std::clamp(mCurrent, mMin, mMax);
		mCurrent = clamp( mCurrent, mMin, mMax );
	}
	
	//	return static_cast<float>(mCurrent)/mMax;
	float rate()const{
		return static_cast<float>(mCurrent)/mMax;
	}
	
	void setValueToMax(){
		mCurrent = mMax;
	}
	
	void setValueToMin(){
		mCurrent = mMin;
	}
	
	operator T() const{
		return mCurrent;
	}

	T getMax()const {
		return mMax;
	}
	

	T getMin()const {
		return mMin;
	}

	T getDefault()const {
		return mDefault;
	}


	RangeNumber( const RangeNumber<T> &n ) = default;
	
	RangeNumber<T>& operator=( const RangeNumber<T> &n ) = default;
	
private:
	T clamp(const T& v, const T& lo, const T& hi )
	{
		return assert(!(hi < lo)),
			v < lo ? lo : hi < v ? hi : v;
	}

private:
	T mCurrent;
	T mDefault;
	T mMax;
	T mMin;
};


