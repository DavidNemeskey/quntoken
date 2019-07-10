#ifndef QX_MODULE_QUEUE_H
#define QX_MODULE_QUEUE_H


#include <vector>
#include <iostream>


enum MODULE_TYPE {
        PREPROC = 0,
        HYPHEN,
        SNT,
        SNTCORR,
        TOKEN,
        CONVXML,
        CONVJSON,
        CONVVERT
};


typedef std::vector<MODULE_TYPE> TYPE_VECTOR;
typedef std::vector<std::istream*> ISTREAM_VECTOR;
typedef std::vector<std::ostream*> OSTREAM_VECTOR;


class QxQueue
{

// members
private:
    TYPE_VECTOR types;
    /**
     * The input streams of the queue modules. The first slot is reserved for
     * the original input stream supplied by the user to \c run(). The rest are
     * <tt>std::stringstream</tt>s, where <tt>istreams[i] == ostreams[i - 1]</tt>.
     */
    ISTREAM_VECTOR istreams;
    /**
     * The output streams of the queue modules. The last slot is reserved for
     * the original output stream supplied by the user to \c run(). The rest are
     * <tt>std::stringstream</tt>s, where <tt>istreams[i + 1] == ostreams[i]</tt>.
     */
    OSTREAM_VECTOR ostreams;

// ctor & dtor
public:
    // ctor:
    QxQueue(TYPE_VECTOR types);

    // dtor:
    ~QxQueue();

// private functions:
private:
    /**
     * Create the streams between the queue elements that store the intermediary
     * results.
     */
    void create_streams();
    /**
     * Delete the streams between the queue elements that store the intermediary
     * results.
     */
    void delete_streams();
    /** Clears the content and state of the results streams. */
    void clear_streams();

// public functions:
public:
    void run(std::istream& inp, std::ostream& out=std::cout);

};


#endif // QX_MODULE_QUEUE_H

