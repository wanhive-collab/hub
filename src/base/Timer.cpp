/*
 * Timer.cpp
 *
 * Milliseconds precision stopwatch
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Timer.h"
#include "SystemException.h"
#include "ds/Twiddler.h"
#include <cerrno>
#include <climits>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>

namespace {
//Nanoseconds in a Second
constexpr long NS_IN_SEC = 1000000000L;
//Microseconds in a Second
constexpr long MS_IN_SEC = 1000000L;
//Milliseconds in a Second
constexpr long MILS_IN_SEC = 1000L;
//Nanoseconds in a Microsecond
constexpr long NS_IN_MS = (NS_IN_SEC / MS_IN_SEC);
//Nanoseconds in a Millisecond
constexpr long NS_IN_MILS = (NS_IN_SEC / MILS_IN_SEC);
//Microseconds in a Millisecond
constexpr long MS_IN_MILS = (MS_IN_SEC / MILS_IN_SEC);

void milsToSpec(unsigned int milliseconds, timespec &ts) {
	ts.tv_sec = milliseconds / MILS_IN_SEC;
	ts.tv_nsec = (milliseconds - (ts.tv_sec * MILS_IN_SEC)) * NS_IN_MILS;
}

long long specToMils(timespec &ts) {
	return (((long long) ts.tv_sec * MILS_IN_SEC) + (ts.tv_nsec / NS_IN_MILS));
}

}  // namespace

namespace wanhive {

Timer::Timer() noexcept {
	now();
}

Timer::~Timer() {

}

double Timer::elapsed() const noexcept {
	return difference(t, currentTime());
}

void Timer::now() noexcept {
	t = currentTime();
}

bool Timer::hasTimedOut(unsigned int milliseconds,
		unsigned int nanoseconds) const noexcept {
	if (milliseconds || nanoseconds) {
		auto diff = ((unsigned long long) milliseconds) * MS_IN_MILS
				+ (nanoseconds / NS_IN_MS);
		auto mark = currentTime();
		return (mark < t) || ((mark - t) > diff);
	} else {
		return true;
	}
}

void Timer::sleep(unsigned int milliseconds, unsigned int nanoseconds) noexcept {
	struct timespec tv;
	//These many seconds
	tv.tv_sec = milliseconds / MILS_IN_SEC;
	//These many nanoseconds
	auto nsec = (unsigned long long) ((milliseconds % MILS_IN_SEC) * NS_IN_MILS)
			+ nanoseconds;
	//Adjust for nanoseconds overflow
	tv.tv_sec += (nsec / NS_IN_SEC);
	tv.tv_nsec = (nsec % NS_IN_SEC);

	//Sleep might get interrupted due to signal delivery
	while (nanosleep(&tv, &tv) == -1 && errno == EINTR) {
		//Loop if interrupted by a signal
	}
}

size_t Timer::refractorTime(char *buffer, size_t size,
		const char *format) noexcept {
	format = format ? format : "%Y-%m-%d %H:%M:%S";
	auto timep = time(nullptr);
	struct tm t;
	return strftime(buffer, size, format, localtime_r(&timep, &t));
}

unsigned long long Timer::timeSeed() noexcept {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return Twiddler::FVN1aHash(&ts, sizeof(ts));
}

int Timer::openTimerfd(bool blocking) {
	auto fd = timerfd_create(CLOCK_MONOTONIC, blocking ? 0 : TFD_NONBLOCK);
	if (fd != -1) {
		return fd;
	} else {
		throw SystemException();
	}
}

void Timer::setTimerfd(int fd, unsigned int expiration, unsigned int interval) {
	struct itimerspec time;
	milsToSpec(expiration, time.it_value);
	milsToSpec(interval, time.it_interval);
	if (timerfd_settime(fd, 0, &time, nullptr)) {
		throw SystemException();
	}
}

void Timer::getTimerfdSettings(int fd, unsigned int &expiration,
		unsigned int &interval) {
	struct itimerspec time;
	if (timerfd_gettime(fd, &time) == 0) {
		expiration = specToMils(time.it_value);
		interval = specToMils(time.it_interval);
	} else {
		throw SystemException();
	}
}

int Timer::closeTimerfd(int fd) noexcept {
	return close(fd);
}

unsigned long long Timer::currentTime() noexcept {
	timespec ts;
	//Cannot fail, returned value ignored
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (((unsigned long long) ts.tv_sec * MS_IN_SEC)
			+ (ts.tv_nsec / NS_IN_MS));
}

double Timer::difference(unsigned long long start,
		unsigned long long end) noexcept {
	return ((double) (end - start) / (double) MS_IN_SEC);
}

} /* namespace wanhive */
