/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ThreadPool.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 20:40:00 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/11 20:59:46 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
using namespace std;

class ThreadPool
{
	public:

		ThreadPool(size_t num_threads = thread::hardware_concurrency())
		{
			for (size_t i = 0; i < num_threads; ++i)
			{
				threads_.emplace_back([this]
				{
					while (true)
					{
						function<void()> task;
						{
							unique_lock<mutex> lock(queue_mutex_);
							cv_.wait(lock, [this]
							{
								return (!tasks_.empty() || stop_);
							});

							if (stop_ && tasks_.empty())
								return ;
							task = move(tasks_.front());
							tasks_.pop();
						}
						task();
					}
				});
			}
		}

		~ThreadPool()
		{
			{
				unique_lock<mutex> lock(queue_mutex_);
				stop_ = true;
			}
			cv_.notify_all();
			for (auto& thread : threads_)
				thread.join();
		}

		void enqueue(function<void()> task)
		{
			{
				unique_lock<std::mutex> lock(queue_mutex_);
				tasks_.emplace(move(task));
			}
			cv_.notify_one();
		}

	private:
		vector<thread>				threads_;
		queue<function<void()> >	tasks_;
		mutex						queue_mutex_;
		condition_variable			cv_;
		bool						stop_ = false;	
};


//https://www.geeksforgeeks.org/cpp/thread-pool-in-cpp/