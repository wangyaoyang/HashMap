#pragma once
#include <list>

using namespace std;

#ifndef	DWORD
#define	DWORD	unsigned long
#endif
#ifndef	WORD
#define	WORD	unsigned short
#endif
#ifndef	BYTE
#define	BYTE	unsigned char
#endif

class CHashPerson;
class CHashHouse;
class CHashStreet;
typedef list<CHashPerson*> HASH_HOUSE;

class CHashPerson
{
private:
	char*	m_pBody;
	int		m_nSize;
public:
	CHashPerson();
	CHashPerson(char* body,int size);
	DWORD	m_Hash();
	void	m_Set(char* body,int size);	//return hash code
	char*	m_Get(int& size);
	bool	m_Equal(CHashPerson& target);
public:
	virtual bool operator == (CHashPerson& target);	//
};

class CHashHouse
{
private:
	HASH_HOUSE		m_staff;
public:
	CHashHouse();
	~CHashHouse();
	BYTE			m_Address();
	CHashPerson*	m_Find(CHashPerson& target);
	bool			m_Add(CHashPerson* person);
};

class CHashStreet
{	//Max 256 houses per street
private:
	CHashHouse*		m_houses;
public:
	CHashStreet();
	~CHashStreet();
	CHashHouse*		m_Find(BYTE house_code);
	bool			m_Add(CHashPerson* person,DWORD id);
};

class CHashCity
{	//Max 256 streets per city
	CHashStreet*	m_streets;		// -- the third byte
public:
	CHashCity(void);
	~CHashCity();
	CHashStreet*	m_Find(BYTE street_code);
	bool			m_Add(CHashPerson* person,DWORD id);
};

class CHashContory
{	//There are 256 cities per contory
private:
	CHashCity*		m_cities;	// -- the second byte
public:
	CHashContory(void);
	~CHashContory();
	void			m_Clear();
	CHashCity*		m_Find(BYTE city_code);
	bool			m_Add(CHashPerson* person,DWORD id);
};

class CHashMapping
{	//There are 256 contories in the world
private:
	CHashContory	m_contories[256];	// -- the first byte in the high WORD
public:
	CHashMapping(void);
	~CHashMapping();
	CHashPerson*	m_Find(CHashPerson& target);
	bool			m_Add(CHashPerson* person);
};
