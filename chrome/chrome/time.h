#ifndef TIME_H
#define TIME_H
#include "basictypes.h"

class Time;
class TimeTicks;

class TimeDelta
{
public:
	TimeDelta() : delta_(0) {
	}

	static TimeDelta FromDays(int64 days);
	static TimeDelta FromHours(int64 hours);
	static TimeDelta FromMinutes(int64 minutes);
	static TimeDelta FromSeconds(int64 secs);
	static TimeDelta FromMilliseconds(int64 ms);
	static TimeDelta FromMicroseconds(int64 us);

	static TimeDelta FromInternalValue(int64 delta) {
		return TimeDelta(delta);
	}

	int64 ToInternalValue() const {
		return delta_;
	}

	int InDays() const;
	int InHours() const;
	int InMinutes() const;
	double InSecondsF() const;
	int64 InSeconds() const;
	double InMillisecondsF() const;
	int64 InMilliseconds() const;
	int64 InMillisecondsRoundedUp() const;
	int64 InMicroseconds() const;

	TimeDelta& operator=(const TimeDelta &other) {
		if(this == &other)
			return *this;
		delta_ = other.delta_;
		return *this;
	}

	// Computations with other deltas.
	TimeDelta operator+(const TimeDelta &other) const {
		return TimeDelta(delta_ + other.delta_);
	}
	TimeDelta operator-(const TimeDelta &other) const {
		return TimeDelta(delta_ - other.delta_);
	}

	TimeDelta& operator+=(const TimeDelta &other) {
		delta_ += other.delta_;
		return *this;
	}
	TimeDelta& operator-=(const TimeDelta &other) {
		delta_ -= other.delta_;
		return *this;
	}
	TimeDelta operator-() const {
		return TimeDelta(-delta_);
	}

	// Computations with ints, note that we only allow multiplicative operations
	// with ints, and additive operations with other deltas.
	TimeDelta operator*(int64 a) const {
		return TimeDelta(delta_ * a);
	}
	TimeDelta operator/(int64 a) const {
		return TimeDelta(delta_ / a);
	}
	TimeDelta& operator*=(int64 a) {
		delta_ *= a;
		return *this;
	}
	TimeDelta& operator/=(int64 a) {
		delta_ /= a;
		return *this;
	}
	int64 operator/(const TimeDelta &a) const {
		return delta_ / a.delta_;
	}

	// Defined below because it depends on the definition of the other classes.
	Time operator+(Time t) const;
	TimeTicks operator+(TimeTicks t) const;

	// Comparison operators.
	bool operator==(const TimeDelta &other) const {
		return delta_ == other.delta_;
	}
	bool operator!=(const TimeDelta &other) const {
		return delta_ != other.delta_;
	}
	bool operator<(const TimeDelta &other) const {
		return delta_ < other.delta_;
	}
	bool operator<=(const TimeDelta &other) const {
		return delta_ <= other.delta_;
	}
	bool operator>(const TimeDelta &other) const {
		return delta_ > other.delta_;
	}
	bool operator>=(const TimeDelta &other) const {
		return delta_ >= other.delta_;
	}

private:
	friend class Time;
	friend class TimeTicks;
	friend TimeDelta operator*(int64 a, TimeDelta td);

	explicit TimeDelta(int64 delta_us) : delta_(delta_us) {
	}

	// Delta in microseconds.
	int64 delta_;
};

inline TimeDelta operator*(int64 a, TimeDelta td) {
	return TimeDelta(a * td.delta_);
}



class Time
{
public:
	static const int64 kMillisecondsPerSecond = 1000;
	static const int64 kMicrosecondsPerMillisecond = 1000;
	static const int64 kMicrosecondsPerSecond = kMicrosecondsPerMillisecond *
		kMillisecondsPerSecond;
	static const int64 kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
	static const int64 kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
	static const int64 kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
	static const int64 kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
	static const int64 kNanosecondsPerMicrosecond = 1000;
	static const int64 kNanosecondsPerSecond = kNanosecondsPerMicrosecond *
		kMicrosecondsPerSecond;
public:

	//定义一个时间结构体，类似与win32的SYSTEMTIME或unix的tm
	struct Exploded{
		int year;            //4个数字 "2014"
		int month;           //1表示1月....
		int day_of_week;     //0表示星期天
		int day_of_month;     //(1-31)
		int hour;            //(0-23)
		int minute;          //(0-59)
		int second;          //(0-59)
		int millisecond;     //(0-999)

		bool HasValidValues() const;
	};

	explicit Time(): us_(0){
	}

	//如果Time对象未初始化返回true
	bool is_null() const{
		return 0 == us_ ;
	}

	//返回一个Unxi纪元上的时间 (Jan 1, 1970)
	static Time UnixEpoch();

	static Time Now();


private:

	// The representation of Jan 1, 1970 UTC in microseconds since the
	// platform-dependent epoch.
	static const int64 kTimeTToMicrosecondsOffset;

	//unix时间戳
	int64 us_;
};

// static
inline TimeDelta TimeDelta::FromDays(int64 days){
	return TimeDelta(days * Time::kMicrosecondsPerDay);
}

// static
inline TimeDelta TimeDelta::FromHours(int64 hours){
	return TimeDelta(hours * Time::kMicrosecondsPerHour);
}

// static
inline TimeDelta TimeDelta::FromMinutes(int64 minutes) {
	return TimeDelta(minutes * Time::kMicrosecondsPerMinute);
}

// static
inline TimeDelta TimeDelta::FromSeconds(int64 secs) {
	return TimeDelta(secs * Time::kMicrosecondsPerSecond);
}

// static
inline TimeDelta TimeDelta::FromMilliseconds(int64 ms) {
	return TimeDelta(ms * Time::kMicrosecondsPerMillisecond);
}

// static
inline TimeDelta TimeDelta::FromMicroseconds(int64 us) {
	return TimeDelta(us);
}

class TimeTicks
{
public:
	TimeTicks(): ticks_(0){
	}

	static TimeTicks Now();

	static TimeTicks HighResNow();

	static int64 GetQPCDrifMicroseconds();

	static bool IsHighResClockWorking();

	bool is_null() const{
		return 0 == ticks_;
	}

	static TimeTicks FromInternalValue(int64 ticks){
		return TimeTicks(ticks);
	}

	int64 ToInternalValue() const{
		return ticks_;
	}

	TimeTicks& operator=(const TimeTicks &other){
		if(&other == this)
			return *this;
		ticks_ = other.ticks_;
		return *this;
	}

	bool operator==(const TimeTicks &other) const{
		return ticks_ == other.ticks_;
	}
	bool operator!=(const TimeTicks &other) const {
		return ticks_ != other.ticks_;
	}
	bool operator<(const TimeTicks &other) const {
		return ticks_ < other.ticks_;
	}
	bool operator<=(const TimeTicks &other) const {
		return ticks_ <= other.ticks_;
	}
	bool operator>(const TimeTicks &other) const {
		return ticks_ > other.ticks_;
	}
	bool operator>=(const TimeTicks &other) const {
		return ticks_ >= other.ticks_;
	}

protected:
	friend class TimeDelta;

	explicit TimeTicks(int64 ticks): ticks_(ticks){
	}

	//时钟计数以微秒为单位
	int64 ticks_;
};


#endif  //TIME_H