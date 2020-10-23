/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_THREADPOOL_H
#define HHUOS_THREADPOOL_H

#include <kernel/thread/KernelThread.h>
#include "lib/util/Queue.h"
#include "lib/util/ArrayBlockingQueue.h"
#include "lib/async/Spinlock.h"
#include "kernel/thread/Thread.h"

class ThreadPool {

private:

    struct ThreadPoolWorker : public Kernel::KernelThread {

        bool isWorking = false;

        ThreadPool *pool = nullptr;

        Spinlock *workQueueLock = nullptr;

        ThreadPoolWorker() : Kernel::KernelThread(Kernel::System::getKernelProcess()) {}

        explicit ThreadPoolWorker(ThreadPool *pool, Spinlock *workQueueLock) : Kernel::KernelThread(Kernel::System::getKernelProcess()), pool(pool), workQueueLock(workQueueLock) {

        }

        ThreadPoolWorker(const ThreadPoolWorker &other) : Kernel::KernelThread(Kernel::System::getKernelProcess()) {
            this->pool = other.pool;
            this->workQueueLock = other.workQueueLock;
        }

        ThreadPoolWorker& operator=(const ThreadPoolWorker &other) {
            this->pool = other.pool;
            this->workQueueLock = other.workQueueLock;

            return *this;
        }

        void run() override {
            while(true) {
                while(pool->workQueue.isEmpty()) {
                    yield();
                }

                workQueueLock->acquire();

                void (*work)() = pool->workQueue.pop();

                workQueueLock->release();

                isWorking = true;
                work();
                isWorking = false;
            }
        }

    };

private:

    Util::ArrayBlockingQueue<void (*)()> workQueue;
    Util::Array<ThreadPoolWorker> threads;

    Spinlock workQueueLock;

    bool hasStarted = false;

public:

    explicit ThreadPool(uint32_t size = 16);

    ~ThreadPool();

    void addWork(void (*func)());

    void startWorking();

    void stopWorking(bool force = false);

};

#endif
