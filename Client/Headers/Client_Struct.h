#pragma once



typedef struct tagBaseDesc
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vSize;

	tagBaseDesc()
	{
		ZeroMemory(this, sizeof(tagBaseDesc));
	};
	tagBaseDesc(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vSize)
	{
		vPos = _vPos;
		vSize = _vSize;
	}

}BASEDESC;

typedef	struct tagRay
{
	D3DXVECTOR3 vOrigin;
	D3DXVECTOR3 vDir;

	tagRay() {};
	tagRay(D3DXVECTOR3 _vOrigin, D3DXVECTOR3 _vDir)
	{
		vOrigin = _vOrigin;
		vDir = _vDir;
	}
}MYRAY;

typedef struct tagWaypoint
{
	D3DXVECTOR3		vPosition;
	float			fRadius = 0.f;
	bool			bOccupied = false;
	bool			bHide = false;
	unsigned int	iNumber = 0;
}WAYPOINT;