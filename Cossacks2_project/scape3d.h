/*****************************************************************************/
/*	File:	Scape3D.h
/*	Desc:	
/*	Author:	Ruslan Shestopalyuk
/*	Date:	14.11.2002
/*****************************************************************************/
#ifndef __SCAPE3D_H__
#define __SCAPE3D_H__
void FillGroundZBuffer();



const int		c_QuadHorzTris		= 16;

const float		c_WorldGridStepX		= 32.0f;
const float		c_WorldGridStepY		= 32.0f;
const float		c_HalfWorldGridStepY	= c_WorldGridStepY / 2.0f;  

const int		c_HmapGridStepX			= 32;
const int		c_HmapGridStepY			= 32;
const int		c_HalfHmapGridStepY		= c_HmapGridStepY / 2;

const float		c_HmapToWorldX			= c_WorldGridStepX / (float)c_HmapGridStepX;
const float		c_HmapToWorldY			= c_WorldGridStepY / (float)c_HmapGridStepY;

const int		c_QuadSide			= c_QuadHorzTris * c_HmapGridStepX;
const int		c_2QuadSide			= c_QuadSide * 2;

const int		c_QuadVert	= (c_QuadHorzTris + 1) * (c_QuadHorzTris + 1);
const int		c_QuadInd	= c_QuadHorzTris * c_QuadHorzTris * 6; 

const float		c_Cos30 = 0.86602540378443864676372317075294f;
const float		c_InvCos30 = 1.15470053837925152901f;
const float		c_Tan30 = 0.57735026918962576450914878050196f;

const float		c_CameraDistance = 3000.0f;
const float		c_MaxMapHeight	= 1024.0f;
const float		c_BottomFieldExtent = c_MaxMapHeight / c_Tan30;

#pragma pack( push )
#pragma pack( 8 )

extern IRenderSystem*	IRS;
extern int RealLx, RealLy, mapx, mapy;
extern short* THMap;
int GetHeight(int x,int y);

float GetGround_d2x( int x, int y );

enum ShadowMode
{
	shmNoShadow		= 0,
	shmShadowPatch	= 1,
	shmShadowVolume	= 2,
	shmSimpleShadow	= 3
}; // enum ShadowMode
	
/*****************************************************************************/
/*	Class:	Rect
/*	Desc:	Rectangle
/*****************************************************************************/
template <class T>
class Rect
{
public:
	T				x;
	T				y;
	T				x1;
	T				y1;

	Rect() : x(0), y(0), x1(0), y1(0) {}
	Rect( T _x, T _y, T _x1, T _y1 ) : x(_x), y(_y), x1(_x1), y1(_y1) {}
	Rect( const Rect& rct ) { x = rct.x; y = rct.y; x1 = rct.x1; y1 = rct.y1; } 

	bool			degenerate	()
	{
		return (x > x1 || y > y1);
	}

	bool			intersect	( const Rect& a, const Rect& b )
	{
		x	= max( a.x,		b.x  );
		y	= max( a.y,		b.y  );
		x1	= min( a.x1,	b.x1 );
		y1	= min( a.y1,	b.y1 );
		if (degenerate()) return false;
		return true;
	}
	T				sqrArea		() const
	{
		return (x1 - x) * (y1 - y);
	}
	void			add			( const Rect& a, const Rect& b )
	{
		x	= min( a.x,		b.x  );
		y	= min( a.y,		b.y  );
		x1	= max( a.x1,	b.x1 );
		y1	= max( a.y1,	b.y1 );
	}
	bool			inside		( const Rect& a )
	{
		return ( x >= a.x && y >= a.y && x1 <= a.x1 && y1 <= a.y1 );
	}
}; // class Rect

typedef Rect<int>	NRect;

/*****************************************************************************/
/*  Class:   Hache
/*  Desc:    Hash table with cached elements allocated from the pool
/*****************************************************************************/
template <class EType, int tableSize, int minPoolSize>
class Hache
{
    //  internal class Entry
    class Entry : public EType
    {
    public:
        Entry() : next(0), prev(0), factor(0), EType() {}

        //  entrys with the same hash values are stored in the double-linked
        //  list chains
        Entry* next;
        Entry* prev;
        unsigned int    factor;
    }; // class Entry

    Entry* pool;               //  pool for elements
    int             poolSize;           //  current pool size
    int             nEntriesUsed;       //  current used number of pool entries
    Entry* table[tableSize];   //  hash table

public:
    Hache()
    {
        pool = new Entry[minPoolSize];
        poolSize = minPoolSize;
        nEntriesUsed = 0;
        memset(table, 0, tableSize * sizeof(Entry*));
    }

    virtual ~Hache()
    {
        delete[]pool;
    }

    void        reset()
    {
        nEntriesUsed = 0;
        memset(table, 0, tableSize * sizeof(Entry*));
    }

    EType* getEl(const typename EType::Attr& attr)
    {
        unsigned int h = attr.hash() % tableSize;
        if (table[h] == 0) return 0;
        //  ok, one or bunch of entries with same hash value
        return locateEntry(*(table[h]), attr);
    }

    EType* getHitAllocEl(const typename EType::Attr& attr, unsigned int factor)
    {
        unsigned int h = attr.hash() % tableSize;
        Entry* entry;
        if (table[h] == 0)
        {
            entry = allocEntry();
            entry->next = 0;
            entry->prev = 0;
            table[h] = entry;
        }
        else
        {
            entry = locateEntry(*(table[h]), attr);
            if (!entry)
            {
                entry = allocEntry();
                entry->next = table[h];
                entry->prev = 0;
                if (table[h]) table[h]->prev = entry;
                table[h] = entry;
            }
        }

        entry->factor = factor;
        return entry;
    }

    int         numElem() const { return nEntriesUsed; }
    int         maxElem() const { return poolSize; }

    EType* elem(int idx)
    {
        assert(idx >= 0 && idx < poolSize);
        return &(pool[idx]);
    }

    int getSizeBytes() const
    {
        int sz = 0;
        for (int i = 0; i < poolSize; i++)
        {
            sz += pool[i].getSizeBytes();
            sz += sizeof(Entry) - sizeof(EType);
        }
        return sz + sizeof(this)
            + tableSize * sizeof(Entry*);
    }

protected:
    Entry* allocEntry()
    {
        if (nEntriesUsed == poolSize)
            //  no more entries, need to dismiss one 
        {
            unsigned int minF = pool[0].factor;
            int dismIdx = 0;
            for (int i = 1; i < poolSize; i++)
            {
                if (pool[i].factor < minF) { dismIdx = i; minF = pool[i].factor; }
            }
            //  pull dismissed item from chain
            Entry* entry = &(pool[dismIdx]);

            if (entry->prev)
            {
                entry->prev->next = entry->next;
            }
            else
                //  dismissed entry is at the chain root 
                //  and is pointed from hash table
            {
                unsigned int h = entry->hash() % tableSize;
                assert(entry == table[h]);
                table[h] = entry->next;
            }

            if (entry->next)
            {
                entry->next->prev = entry->prev;
            }

            return entry;
        }
        return &(pool[nEntriesUsed++]);
    }

    Entry* locateEntry(const Entry& first, const typename EType::Attr& attr)
    {
        Entry* cur = const_cast<Entry*>(&first);
        do
        {
            if (cur->hasAttr(attr)) return cur;
            cur = cur->next;
        } while (cur);
        return 0;
    }
}; // class Hache

/*****************************************************************************/
/*	Class:	ScapeQuad
/*	Desc:	Single quad of landscape mesh
/*****************************************************************************/
template <class VType>
class ScapeQuad
{
public:
	//  internal class Attr
	class Attr
	{
	public:
		Attr() : qx(0), qy(0), qlod(0) {}
		Attr( int x, int y, int lod ) : qx(x), qy(y), qlod(lod) {}		
		unsigned int hash() const 
		{ 
			unsigned char* s = (unsigned char*) this;
			unsigned int h = 0;
			int nBytes = sizeof( *this );
			for (int i = 0; i < nBytes; i++)
			{
				h = (h * 729 + (*s) * 37) ^ (h >> 1);
				s++;
			}
			return h;
		}
		int			qx;
		int			qy;
		int			qlod;
	}; // Attr

public:
	ScapeQuad();
	
	bool	IsVisible();
	void	CreateInWorld( int qx, int qy, int qlod );
	void	CreateTIN( int qx, int qy, int qlod );
	void	InitMesh();

	bool hasAttr( const Attr& attr ) const 
			{ return (lod == attr.qlod) && (attr.qx == bound.x) && (attr.qy == bound.y); }

	int		getSizeBytes() const 
	{  
		return sizeof( *this ) + mesh.getSizeBytes() - sizeof( BaseMesh );
	}

	unsigned int hash() const 
	{
		return Attr( bound.x, bound.y, lod ).hash();
	}

protected:
	static void		InitIndices();	
	static  WORD	qIdx[c_QuadInd];

	BaseMesh		mesh;

private:
	friend class ScapeMesh;
	friend class ShadowPatch;
	friend class ShadowMapper;

	//  bounding box
	NRect	bound;
	int		maxH;	//  maximal height of ground surface in the quad
	int		lod;	//  level of quad's detail
}; // ScapeQuad 

const int c_QuadHashTableSize	= 541; // 100th prime number
const int c_QuadPoolSize		= 256;

/*****************************************************************************/
/*	Class:	ScapeMesh
/*	Desc:	Represents landscape mold mesh for filling z-buffer values
/*****************************************************************************/
class ScapeMesh
{
public:
	typedef     ScapeQuad<Vertex2t> TScapeQuad;

	void		Init( IRenderSystem* iRS );
	void		Show( IRenderSystem* iRS );
	void		Update();
	void		reset();
	void		setNeedUpdate( bool need = true ) { needUpdate = need; }

protected:
	Hache<TScapeQuad, c_QuadHashTableSize, c_QuadPoolSize>	quadHache;
	NRect		cachedBounds;
	bool		needUpdate;
	Matrix4D	skewMatr;

private:
	friend class ShadowMapper;
}; // class ScapeMesh

const int	c_MinShadowPatchSide		= 64;
const int	c_MaxShadowPatchSide		= 512;
const int	c_nMinShadowPatchGridStep	= 8;
const float	c_fMinShadowPatchGridStep	= (float)c_nMinShadowPatchGridStep;

const int	c_SPSideMaxTris			= 16;
const int	c_ShadowPatchVert		= (c_SPSideMaxTris + 1) * (c_SPSideMaxTris + 1);
const int	c_ShadowPatchInd		= c_SPSideMaxTris * c_SPSideMaxTris * 6; 

/*****************************************************************************/
/*	Class:	ShadowPatch
/*	Desc:	Represents piece of static world geometry (landscape)
/*				which works as shadow receiver
/*****************************************************************************/
class ShadowPatch
{
public:
	//  internal class Attr
	class Attr
	{
	public:
		Attr() : px(0), py(0), pside(c_MinShadowPatchSide) {}
		Attr( int x, int y, int lod ) : px(x), py(y), pside(lod) {}		
		
		unsigned int hash() const 
		{ 
			unsigned char* s = (unsigned char*) this;
			unsigned int h = 0;
			int nBytes = sizeof( *this );
			for (int i = 0; i < nBytes; i++)
			{
				h = (h * 729 + (*s) * 37) ^ (h >> 1);
				s++;
			}
			return h;
		}

		bool equal( const Attr& attr ) const
		{
			return (px == attr.px) && (py == attr.py) && (pside == attr.pside);
		}

		void reset() 
		{
			px = py = pside = 0;
		}

		int			px;
		int			py;
		int			pside;
	}; // Attr

	void InitMesh( IRenderSystem* irs );
	void CreateMesh( const Camera& lightCam, int qx, int qy, int qside );

	bool hasAttr( const Attr& _attr ) const 
			{ return _attr.equal( attr ); }

	void resetAttr() 
	{
		attr.reset();
	}

	unsigned int hash() const 
	{
		return attr.hash();
	}

	
protected:
	BaseMesh		mesh;
	float			begWorldX, begWorldY;
	float			worldSide;

private:
	friend class ShadowMapper;
	

	Attr			attr;
}; // class ShadowPatch

const float c_LightDistance				= 1000.0f;
const float c_DefObjectRadius			= 180.0f; 
const int	c_MapTexSide				= 256;
const int	c_ShadowPatchHashTableSize	= 541;
const int	c_ShadowPatchPoolSize		= 128;

class AnimModel;
/*****************************************************************************/
/*	Class:	ShadowMapper
/*	Desc:	Performs shadow casting
/*****************************************************************************/
class ShadowMapper
{
	Hache<	ShadowPatch, 
			c_ShadowPatchHashTableSize, 
			c_ShadowPatchPoolSize>			patchHache;

	Camera			lightCam;
	Vector3D		lightDir;
	int				mapTex;
	TextureDescr	mapTD;
	ViewPort		vport;
	int				shMap, shReceiver;	
	Matrix4D		invLightView;

public:
	void		init		( IRenderSystem* irs );
	void		castShadow	( IRenderSystem* irs, AnimModel& mdl, Matrix4D& transf );
	void		reset		();
}; // class ShadowMapper



#pragma pack ( pop )

#endif // __SCAPE3D_H__