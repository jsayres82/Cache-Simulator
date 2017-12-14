#ifndef BUSREQUEST_H
#define BUSREQUEST_H





class BusRequest
{
public:
	typedef enum {BusRd, BusRdX, BusUpd} BusMessage; 
	BusRequest::BusMessage command;
	int lineSet;
	int lineTag;
	int cycleCost;
	bool sharedLine;
	unsigned long long address;
	BusRequest(BusMessage, int, int, int, unsigned long long);
	BusMessage getCommand();
	bool isLineShared();
	void setSharedLine();
	int getSet();
	int getTag();
	int getCycleCost();
	~BusRequest(void);
};

#endif
