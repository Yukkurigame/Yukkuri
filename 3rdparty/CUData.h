/*
 * CUData.h
 *
 *  Created on: 17.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef CUDATA_H_
#define CUDATA_H_

class CUDataUser;

#include <string>
#include "types.h"



class CUData
{
public:

	CUData(CUDataUser* pUser) :
		pUser(pUser), regRef(-1) { }

	CUDataUser* getUser() const { return pUser; }
	void clearUser();
	void onGC();

	const LuaRegRef& getRegRef() const { return regRef; }
	void setRegRef(const LuaRegRef& ref) { regRef = ref; }


private:
	CUDataUser* pUser;

	// Reference to userdata in lua registry. Registry is in standard lua state "lua", created on lua initialization.
	LuaRegRef regRef;

	CUData(CUData& ref);
	CUData& operator=(CUData& ref);
	~CUData();


};


#endif /* CUDATA_H_ */
