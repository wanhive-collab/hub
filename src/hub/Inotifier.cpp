/*
 * Inotifier.cpp
 *
 * File system monitor for Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Inotifier.h"
#include "Hub.h"
#include "../base/SystemException.h"
#include "../reactor/Reactor.h"

namespace wanhive {

Inotifier::Inotifier(bool blocking) :
		offset(0), limit(0) {
	auto fd = inotify_init1(!blocking ? IN_NONBLOCK : 0);
	if (fd != -1) {
		setHandle(fd);
		memset(buffer, 0, sizeof(buffer));
	} else {
		throw SystemException();
	}
}

Inotifier::~Inotifier() {

}

void Inotifier::start() {

}

void Inotifier::stop() noexcept {

}

bool Inotifier::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Inotifier> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Inotifier::publish(void *arg) noexcept {
	return false;
}

int Inotifier::add(const char *pathname, uint32_t mask) {
	auto wd = inotify_add_watch(getHandle(), pathname, mask);
	if (wd != -1) {
		return wd;
	} else {
		throw SystemException();
	}
}

void Inotifier::remove(int identifier) {
	if (inotify_rm_watch(getHandle(), identifier)) {
		throw SystemException();
	}
}

ssize_t Inotifier::read() {
	//Clear out the old notifications
	offset = 0;
	limit = 0;

	//Read new notifications
	auto nRead = Descriptor::read(buffer, sizeof(buffer));
	if (nRead != -1) {
		limit = nRead;
	}
	return nRead;
}

const InotifyEvent* Inotifier::next() {
	if (limit && offset < limit) {
		auto event = (InotifyEvent*) (buffer + offset);
		offset += sizeof(InotifyEvent) + event->len;
		return event;
	} else {
		return nullptr;
	}
}

} /* namespace wanhive */
