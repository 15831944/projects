#include "time.h"


int TimeDelta::InDays() const {
	return static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
}

int TimeDelta::InHours() const {
	return static_cast<int>(delta_ / Time::kMicrosecondsPerHour);
}

int TimeDelta::InMinutes() const {
	return static_cast<int>(delta_ / Time::kMicrosecondsPerMinute);
}

double TimeDelta::InSecondsF() const {
	return static_cast<double>(delta_) / Time::kMicrosecondsPerSecond;
}

int64 TimeDelta::InSeconds() const {
	return delta_ / Time::kMicrosecondsPerSecond;
}

double TimeDelta::InMillisecondsF() const {
	return static_cast<double>(delta_) / Time::kMicrosecondsPerMillisecond;
}

int64 TimeDelta::InMilliseconds() const {
	return delta_ / Time::kMicrosecondsPerMillisecond;
}

int64 TimeDelta::InMillisecondsRoundedUp() const {
	return (delta_ + Time::kMicrosecondsPerMillisecond - 1) /
		Time::kMicrosecondsPerMillisecond;
}

int64 TimeDelta::InMicroseconds() const {
	return delta_;
}





Time Time::UnixEpoch(){
	Time time;
	time.us_ = kTimeTToMicrosecondsOffset;
	return time;
}



// TimeTicks TimeTicks::Now(){
// 	return TimeTicks() + 
// }


inline bool is_in_range(int value, int lo, int hi)
{
	return lo <= value && hi >= value;
}

bool Time::Exploded::HasValidValues() const
{
	return is_in_range(month, 1, 12)       &&
		   is_in_range(day_of_week, 0, 6)  &&
		   is_in_range(day_of_month, 1,31) &&
		   is_in_range(hour, 0, 23)        &&
		   is_in_range(minute, 0, 59)      &&
		   is_in_range(second, 0 ,59)      &&
		   is_in_range(millisecond, 0, 999);
}