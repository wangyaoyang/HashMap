#include "HashMapping.h"
#include "..\..\TmnCpps\CRC32\CRC32.h"

CHashPerson::CHashPerson() {
	this->m_pBody = NULL;
	this->m_nSize = 0;
}

CHashPerson::CHashPerson(char* body,int size) {
	this->m_pBody = body;
	this->m_nSize = size;
	if (this->m_pBody == NULL || this->m_nSize == 0) {
		this->m_pBody = NULL;
		this->m_nSize = 0;
	}
}

void CHashPerson::m_Set(char* body,int size) {	//return hash code
	if (NULL == body || 0 == size) {
		this->m_pBody = NULL;
		this->m_nSize = 0;
	} else {
		this->m_pBody = body;
		this->m_nSize = size;
	}
}

char* CHashPerson::m_Get(int& size) {
	size = this->m_pBody ? this->m_nSize : 0;
	return this->m_pBody;
}

DWORD CHashPerson::m_Hash() {
	DWORD	nHash = 0;
	CRC32	crc32;
	if (NULL == this->m_pBody || 0 == this->m_nSize) return 0;
	crc32.CRCInit(nHash);
	crc32.CRCUpdate((BYTE*)this->m_pBody, this->m_nSize, nHash);
	crc32.CRCFinal(nHash);
	return nHash;
}

bool CHashPerson::m_Equal(CHashPerson& target) {
	if (0 == this->m_nSize || NULL == this->m_pBody) return false;
	if (0 == target.m_nSize || NULL == target.m_pBody) return false;
	if (this->m_nSize == target.m_nSize &&
		0 == memcmp(this->m_pBody,target.m_pBody,this->m_nSize)) return true;
	return false;
}

bool CHashPerson::operator == (CHashPerson& target) {
	return m_Equal(target);
}

////////////////////////////////////////////////////////////
CHashHouse::CHashHouse() {
}

CHashHouse::~CHashHouse() {
	this->m_staff.clear();
}

CHashPerson* CHashHouse::m_Find(CHashPerson& target) {
	if (!this->m_staff.empty()) {
		for (HASH_HOUSE::iterator i = this->m_staff.begin(); i != this->m_staff.end(); ++ i) {
			CHashPerson*	p = (CHashPerson*) (*i);
			if (*p == target) return p;
		}
	}
	return NULL;
}

bool CHashHouse::m_Add(CHashPerson* person) {
	if (NULL != this->m_Find(*person)) return false;	//that guy is already in this house
	this->m_staff.push_back(person);
	return true;	//OK, arrange that guy
}

////////////////////////////////////////////////////////////
CHashStreet::CHashStreet() {
	this->m_houses = NULL;
}

CHashStreet::~CHashStreet() {
	if (NULL != this->m_houses)
		delete [] this->m_houses;
}

CHashHouse* CHashStreet::m_Find(BYTE house_code) {
	if (NULL == this->m_houses) return NULL;
	return (&this->m_houses[house_code]);
}

bool CHashStreet::m_Add(CHashPerson* person,DWORD id) {
	if (this->m_houses == NULL)
		this->m_houses = new CHashHouse[256];
	BYTE	house_code = (BYTE)(0x000000ff & id);
	return this->m_houses[house_code].m_Add(person);
}

////////////////////////////////////////////////////////////

CHashCity::CHashCity() {
	this->m_streets = NULL;
}

CHashCity::~CHashCity() {
	if (NULL != this->m_streets)
		delete [] this->m_streets;
}

CHashStreet* CHashCity::m_Find(BYTE street_code) {
	if (NULL == this->m_streets) return NULL;
	return (&this->m_streets[street_code]);
}

bool CHashCity::m_Add(CHashPerson* person,DWORD id) {
	if (this->m_streets == NULL)
		this->m_streets = new CHashStreet[256];
	BYTE	street_code = (BYTE)(0x000000ff & (id >> 8));
	return this->m_streets[street_code].m_Add(person,id);
}

////////////////////////////////////////////////////////////
CHashContory::CHashContory() {
	this->m_cities = NULL;
}

CHashContory::~CHashContory() {
	this->m_Clear();
}

CHashCity* CHashContory::m_Find(BYTE city_code) {
	if (NULL == this->m_cities) return NULL;
	return (&this->m_cities[city_code]);
}

bool CHashContory::m_Add(CHashPerson* person,DWORD id) {
	if (this->m_cities == NULL)
		this->m_cities = new CHashCity[256];
	BYTE	city_code = (BYTE)(0x000000ff & (id >> 16));
	return this->m_cities[city_code].m_Add(person,id);
}

void CHashContory::m_Clear() {
	if (NULL != this->m_cities)
		delete [] this->m_cities;
	this->m_cities = NULL;
}

CHashMapping::CHashMapping(void)
{
}


CHashMapping::~CHashMapping(void)
{
}

CHashPerson* CHashMapping::m_Find(CHashPerson& target) {
	DWORD		nHash = target.m_Hash();
	BYTE		contory_code	= (BYTE)(0x000000ff & (nHash >> 24));
	BYTE		city_code		= (BYTE)(0x000000ff & (nHash >> 16));
	BYTE		street_code		= (BYTE)(0x000000ff & (nHash >> 8));
	BYTE		house_code		= (BYTE)(0x000000ff & nHash);
	CHashCity*	city = this->m_contories[contory_code].m_Find(city_code);
	if (NULL != city) {
		CHashStreet*	street = city->m_Find(street_code);
		if (NULL != street) {
			CHashHouse*	house = street->m_Find(house_code);
			if (NULL != house) {
				return house->m_Find(target);
			} else return NULL;
		} else return NULL;
	} else return NULL;
}

bool CHashMapping::m_Add(CHashPerson* person) {
	if (NULL != person) {
		DWORD	nHash = person->m_Hash();
		BYTE	contory = (BYTE)(0x000000ff & (nHash >> 24));
		return this->m_contories[contory].m_Add(person,nHash);
	} else return false;
}
