//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "request_queue.h"

    RequestQueue::RequestQueue(const SearchServer& search_server) :
        search_server_(search_server),
        count_req_(0)
        {
        }
    
    int RequestQueue::GetNoResultRequests() const {
        return min_in_day_ - count_req_;
        // напишите реализацию
    }

    void RequestQueue::AddRequest(int num_request) {
        if (num_request != 0) 
        {
            ++current_time_;
            ++count_req_;
        }  
    }
