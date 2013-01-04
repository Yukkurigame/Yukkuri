/*
 * CUDataUser.h
 *
 *  Created on: 17.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef CUDATAUSER_H_
#define CUDATAUSER_H_


class CUData;
struct lua_State;


class CUDataUser
{
public:

	CUDataUser( ) : pUdata(0) { }

	void setUData(CUData* pUdata) { this->pUdata = pUdata; }
	int pushUData( lua_State* L );
	virtual ~CUDataUser( );

	int pushUStorage( lua_State* L );


protected:
	virtual CUData* createUData( ) = 0;

private:
	CUData* pUdata;

};


CUDataUser* getUserData( lua_State* L, int idx );


#endif /* CUDATAUSER_H_ */
