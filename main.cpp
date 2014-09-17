#include <array>
#include <iostream>
#include <sstream>
#include <cstring>


template<size_t S>
class RingBuffer: public std::basic_streambuf<char>
{
	static_assert(S!=0,"Size can't be zero.");
public:
	RingBuffer():_buffer(),_bufferEnd(_buffer.end()-1)
	{
		_buffer.fill('\0');
		setp(_buffer.data(),_buffer.end());
		// beg, next, end
		setg(_buffer.data(),_buffer.data(),_buffer.end());
	}
	RingBuffer(const RingBuffer&)=delete;
	RingBuffer& operator=(const RingBuffer&)=delete;

	// Buffer access
	const char* data() const
	{
		return _buffer.data();
	}

	// Overrides ---------------------------------------------------------------

	// Writing
	virtual std::streamsize xsputn(const char* s, std::streamsize n)
	{
		std::streamsize spaceleft=_bufferEnd-pptr();
		if (spaceleft<=0)
		{
			_M_out_cur=_buffer.data();
			spaceleft=_buffer.size()-1;
		}
		if (spaceleft<n) //todo:do while n>0..
		{
			std::memcpy(pptr(),s,spaceleft);
			std::memcpy(_buffer.data(),s+spaceleft,n-spaceleft);
			_M_out_cur=_buffer.data()+n-spaceleft;
		}
		else
		{
			std::memcpy(pptr(),s,n);
			_M_out_cur+=n;
		}
		return n;
	}

	// Flush
	virtual int sync()
	{
		_buffer.fill('\0');
		return 0;
	};
private:
	std::array<char,S+1> _buffer; // Extra space for 0
	char* _bufferEnd;
};

template<size_t S>
class RingStream : public std::basic_iostream<char>
{
public:
	RingStream():std::basic_iostream<char>(&_ringBuffer),_ringBuffer(){}
	std::string	str() const
	{
		return std::string(_ringBuffer.data());
	}
private:
	RingBuffer<S> _ringBuffer;
};

int main()
{
	RingStream<16> rb;
	std::cout<<"Writing\n";
	for (int i=0; i<11; ++i)
	{
		rb<<"("<<i<<")";
	}
	std::cout<<"Reading\n";
	std::cout<<rb.str();
	return 0;
}
