//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "request_queue.h"

    RequestQueue::RequestQueue(const SearchServer& search_server) :
        search_server_(search_server),
        count_req_(0)
        {
        }
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(result.size());
        return result;
        // напишите реализацию
    }
    
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        auto result = search_server_.FindTopDocuments(raw_query, status);
        AddRequest(result.size());
        return result;
        // напишите реализацию
    }

    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
        auto result = search_server_.FindTopDocuments(raw_query);
        AddRequest(result.size());
        return result;
        // напишите реализацию
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