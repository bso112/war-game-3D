#include "CollisionMgr.h"
#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{

}

HRESULT CCollisionMgr::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CCollisionMgr::Ready_Component(void * pArg)
{
	return S_OK;
}

HRESULT CCollisionMgr::Add_CollisionGroup(COLLISION_GROUP eGroup, CGameObject* pGameObject)
{
	if (COL_END <= eGroup ||
		nullptr == pGameObject)
		return E_FAIL;

	m_CollisionGroup[eGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CCollisionMgr::CheckCollision()
{
	//죽은 오브젝트들은 GameObject 안의 충돌list에서 제거한다.(콜리전매니저의 리스트에서 제거하는게 아니다!)
	for (auto& group : m_CollisionGroup)
	{
		for (auto& go : group)
		{
			go->Clear_DeadObject();
		}
	}

	if (FAILED(CheckCollision_Unit()))
		return E_FAIL;


	if (FAILED(CheckCollision_Weapon()))
		return E_FAIL;

	if (FAILED(CheckCollision_Arrow()))
		return E_FAIL;

	if (FAILED(CheckCollision_Building()))
		return E_FAIL;

	if (FAILED(CheckCollision_SKILL()))
		return E_FAIL;

	if (FAILED(CheckCollision_Gate()))
		return E_FAIL;


	for (size_t i = 0; i < COL_END; ++i)
	{
		for (auto& pGameObject : m_CollisionGroup[i])
			Safe_Release(pGameObject);

		m_CollisionGroup[i].clear();
	}


	return S_OK;
}


HRESULT CCollisionMgr::CheckCollision_Unit()
{

	int iAcc = 0;
	for (size_t i = 0; i < m_CollisionGroup[COL_UNIT].size(); ++i)
	{
		for (size_t j = i + 1; j < m_CollisionGroup[COL_UNIT].size(); ++j)
		{
			CGameObject* pSrc = m_CollisionGroup[COL_UNIT][i];
			CGameObject* pDst = m_CollisionGroup[COL_UNIT][j];
			++iAcc;

#pragma region 충돌컬링

			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;

			_float fDistAB = D3DXVec3Length(&(pTransformA->Get_Position() - pTransformB->Get_Position()));
			_float fAvgSizeA = (pTransformA->Get_Scaled().x + pTransformA->Get_Scaled().y + pTransformA->Get_Scaled().z) / 3;
			_float fAvgSizeB = (pTransformB->Get_Scaled().x + pTransformB->Get_Scaled().y + pTransformB->Get_Scaled().z) / 3;

			//만약 두 객체의 거리가 두 객체의 사이즈의 합 * 2 보다 크면 충돌처리 안함
			if (fDistAB > (fAvgSizeA + fAvgSizeB) * 2)
				continue;
#pragma endregion



			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float  fCollisionLength = 0.f;
			_float3	vPushDir;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, vPushDir))
			{
				//충돌영역을 받아왔을 경우
				if (0.1f < fCollisionLength)
				{

					if (nullptr == pTransformA || nullptr == pTransformB)
						return E_FAIL;

					//트리거 콜라이더가 아니면 밀어낸다.
					if (!pColliderA->IsTrigger() && !pColliderB->IsTrigger())
					{
						//밀어낼 방향을 구하고 (y축으로는 밀지 않는다)
						_float3 vPushDirA, vPushDirB;
						vPushDirA = pTransformA->Get_Position() - pTransformB->Get_Position();
						vPushDirA.y = 0.f;
						vPushDirB = pTransformB->Get_Position() - pTransformA->Get_Position();
						vPushDirB.y = 0.f;
						D3DXVec3Normalize(&vPushDirA, &vPushDirA);
						D3DXVec3Normalize(&vPushDirB, &vPushDirB);

						//민다
						pTransformA->AddForce(vPushDirA * (fCollisionLength * 0.5f));
						pTransformB->AddForce(vPushDirB * (fCollisionLength * 0.5f));
						//pTransformA->AddForce(vPushDirA * (fCollisionLength));

					}

				}

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}

	iAcc = iAcc;
	return S_OK;
}

HRESULT CCollisionMgr::CheckCollision_Weapon()
{
	for (auto& pSrc : m_CollisionGroup[COL_UNIT])
	{
		for (auto& pDst : m_CollisionGroup[COL_WEAPON])
		{

#pragma region 충돌컬링

			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;

			_float fDistAB = D3DXVec3Length(&(pTransformA->Get_Position() - pTransformB->Get_Position()));
			_float fAvgSizeA = (pTransformA->Get_Scaled().x + pTransformA->Get_Scaled().y + pTransformA->Get_Scaled().z) / 3;
			_float fAvgSizeB = (pTransformB->Get_Scaled().x + pTransformB->Get_Scaled().y + pTransformB->Get_Scaled().z) / 3;

			//만약 두 객체의 거리가 두 객체의 사이즈의 합 * 2 보다 크면 충돌처리 안함
			if (fDistAB > (fAvgSizeA + fAvgSizeB) * 2)
				continue;
#pragma endregion


			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			_float3 vPushDir;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, vPushDir))
			{

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}

		}
	}
	return S_OK;
}

HRESULT CCollisionMgr::CheckCollision_Arrow()
{
	for (auto& pSrc : m_CollisionGroup[COL_UNIT])
	{
		for (auto& pDst : m_CollisionGroup[COL_ARROW])
		{

#pragma region 충돌컬링

			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;

			_float fDistAB = D3DXVec3Length(&(pTransformA->Get_Position() - pTransformB->Get_Position()));
			_float fAvgSizeA = (pTransformA->Get_Scaled().x + pTransformA->Get_Scaled().y + pTransformA->Get_Scaled().z) / 3;
			_float fAvgSizeB = (pTransformB->Get_Scaled().x + pTransformB->Get_Scaled().y + pTransformB->Get_Scaled().z) / 3;

			//만약 두 객체의 거리가 두 객체의 사이즈의 합 * 2 보다 크면 충돌처리 안함
			if (fDistAB > (fAvgSizeA + fAvgSizeB) * 2)
				continue;
#pragma endregion


			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			_float3 vPushDir;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, vPushDir))
			{

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}

	for (auto& pSrc : m_CollisionGroup[COL_TOWER])
	{
		for (auto& pDst : m_CollisionGroup[COL_ARROW])
		{

#pragma region 충돌컬링

			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;

			_float fDistAB = D3DXVec3Length(&(pTransformA->Get_Position() - pTransformB->Get_Position()));
			_float fAvgSizeA = (pTransformA->Get_Scaled().x + pTransformA->Get_Scaled().y + pTransformA->Get_Scaled().z) / 3;
			_float fAvgSizeB = (pTransformB->Get_Scaled().x + pTransformB->Get_Scaled().y + pTransformB->Get_Scaled().z) / 3;

			//만약 두 객체의 거리가 두 객체의 사이즈의 합 * 2 보다 크면 충돌처리 안함
			if (fDistAB > (fAvgSizeA + fAvgSizeB) * 2)
				continue;
#pragma endregion


			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			_float3 vPushDir;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, vPushDir))
			{

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}
	return S_OK;
}

HRESULT CCollisionMgr::CheckCollision_Building()
{
	for (auto& pSrc : m_CollisionGroup[COL_UNIT])
	{
		for (auto& pDst : m_CollisionGroup[COL_BUILDING])
		{

#pragma region 충돌컬링

			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;

			_float fDistAB = D3DXVec3Length(&(pTransformA->Get_Position() - pTransformB->Get_Position()));
			_float fAvgSizeA = (pTransformA->Get_Scaled().x + pTransformA->Get_Scaled().y + pTransformA->Get_Scaled().z) / 3;
			_float fAvgSizeB = (pTransformB->Get_Scaled().x + pTransformB->Get_Scaled().y + pTransformB->Get_Scaled().z) / 3;

			//만약 두 객체의 거리가 두 객체의 사이즈의 합 * 2 보다 크면 충돌처리 안함
			if (fDistAB > (fAvgSizeA + fAvgSizeB) * 2)
				continue;
#pragma endregion


			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			_float3 vPushDir;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, vPushDir))
			{

				//충돌영역을 받아왔을 경우
				if (0.1f < fCollisionLength)
				{

					if (nullptr == pTransformA || nullptr == pTransformB)
						return E_FAIL;

					//트리거 콜라이더가 아니면 밀어낸다.
					if (!pColliderA->IsTrigger() && !pColliderB->IsTrigger())
					{

						_float3	vSrcPos = pTransformA->Get_Position();
						_float3	vSrcSize = pTransformA->Get_Scaled();
						_float3 vDstPos = pTransformB->Get_Position();
						_float3	vDstSize = pTransformB->Get_Scaled();

						////밀어낼 방향을 구하고 (y축으로는 밀지 않는다)
						//_float3 vPushDirA;
						//vPushDirA = vSrcPos - vDstPos;
						//D3DXVec3Normalize(&vPushDirA, &vPushDirA);
						//vPushDirA.y = 0;
						////민다(Src만)

						_float fSrcLength = D3DXVec3Dot(&vSrcPos, &vPushDir);
						_float fDstLength = D3DXVec3Dot(&vDstPos, &vPushDir);
						if (fSrcLength < fDstLength)
							vPushDir *= -1;
						pTransformA->AddForce(vPushDir * fCollisionLength);

					}

				}

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}
	return S_OK;
}

HRESULT CCollisionMgr::CheckCollision_Gate()
{
	CheckCollision_NoneMove(COL_UNIT, COL_GATE);
	CheckCollision_NoneMove(COL_ARROW, COL_GATE);
	CheckCollision_NoneMove(COL_WEAPON, COL_GATE);


	return S_OK;
}

HRESULT CCollisionMgr::CheckCollision_SKILL()
{
	for (auto& pSrc : m_CollisionGroup[COL_UNIT])
	{
		for (auto& pDst : m_CollisionGroup[COL_SKILL])
		{


			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;



			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, _float3()))
			{

				//충돌영역을 받아왔을 경우
				if (0.1f < fCollisionLength)
				{

					if (nullptr == pTransformA || nullptr == pTransformB)
						return E_FAIL;

					//트리거 콜라이더가 아니면 밀어낸다.
					if (!pColliderA->IsTrigger() && !pColliderB->IsTrigger())
					{

						_float3	vSrcPos = pTransformA->Get_Position();
						_float3	vSrcSize = pTransformA->Get_Scaled();
						_float3 vDstPos = pTransformB->Get_Position();
						_float3	vDstSize = pTransformB->Get_Scaled();

						//밀어낼 방향을 구하고 (y축으로는 밀지 않는다)
						_float3 vPushDirA;
						vPushDirA = vSrcPos - vDstPos;
						D3DXVec3Normalize(&vPushDirA, &vPushDirA);
						vPushDirA.y = 0;
						//민다(Src만)
						pTransformA->AddForce(vPushDirA * fCollisionLength);

					}

				}

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}

	for (auto& pSrc : m_CollisionGroup[COL_BUILDING])
	{
		for (auto& pDst : m_CollisionGroup[COL_SKILL])
		{


			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;



			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, _float3()))
			{

				//충돌영역을 받아왔을 경우
				if (0.1f < fCollisionLength)
				{

					if (nullptr == pTransformA || nullptr == pTransformB)
						return E_FAIL;

					//트리거 콜라이더가 아니면 밀어낸다.
					if (!pColliderA->IsTrigger() && !pColliderB->IsTrigger())
					{

						_float3	vSrcPos = pTransformA->Get_Position();
						_float3	vSrcSize = pTransformA->Get_Scaled();
						_float3 vDstPos = pTransformB->Get_Position();
						_float3	vDstSize = pTransformB->Get_Scaled();

						//밀어낼 방향을 구하고 (y축으로는 밀지 않는다)
						_float3 vPushDirA;
						vPushDirA = vSrcPos - vDstPos;
						D3DXVec3Normalize(&vPushDirA, &vPushDirA);
						vPushDirA.y = 0;
						//민다(Src만)
						pTransformA->AddForce(vPushDirA * fCollisionLength);

					}

				}

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}


	return S_OK;



}

HRESULT CCollisionMgr::CheckCollision_NoneMove(COLLISION_GROUP eMove, COLLISION_GROUP eNoneMove)
{
	for (auto& pSrc : m_CollisionGroup[eMove])
	{
		for (auto& pDst : m_CollisionGroup[eNoneMove])
		{

#pragma region 충돌컬링

			CTransform* pTransformA = (CTransform*)pSrc->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pDst->Find_Component(L"Com_Transform");

			if (nullptr == pTransformA || nullptr == pTransformB)
				continue;

			_float fDistAB = D3DXVec3Length(&(pTransformA->Get_Position() - pTransformB->Get_Position()));
			_float fAvgSizeA = (pTransformA->Get_Scaled().x + pTransformA->Get_Scaled().y + pTransformA->Get_Scaled().z) / 3;
			_float fAvgSizeB = (pTransformB->Get_Scaled().x + pTransformB->Get_Scaled().y + pTransformB->Get_Scaled().z) / 3;

			//만약 두 객체의 거리가 두 객체의 사이즈의 합 * 2 보다 크면 충돌처리 안함
			if (fDistAB > (fAvgSizeA + fAvgSizeB) * 2)
				continue;
#pragma endregion


			CCollider* pColliderA = (CCollider*)pSrc->Find_Component(L"Com_Collider");
			CCollider* pColliderB = (CCollider*)pDst->Find_Component(L"Com_Collider");

			if (nullptr == pColliderA ||
				nullptr == pColliderB)
				return E_FAIL;

			_float fCollisionLength = 0.f;
			if (pColliderA->Check_Collision(pColliderB, fCollisionLength, _float3()))
			{

				//충돌영역을 받아왔을 경우
				if (0.1f < fCollisionLength)
				{

					if (nullptr == pTransformA || nullptr == pTransformB)
						return E_FAIL;

					//트리거 콜라이더가 아니면 밀어낸다.
					if (!pColliderA->IsTrigger() && !pColliderB->IsTrigger())
					{

						_float3	vSrcPos = pTransformA->Get_Position();
						_float3	vSrcSize = pTransformA->Get_Scaled();
						_float3 vDstPos = pTransformB->Get_Position();
						_float3	vDstSize = pTransformB->Get_Scaled();

						//밀어낼 방향을 구하고 (y축으로는 밀지 않는다)
						_float3 vPushDirA;
						vPushDirA = vSrcPos - vDstPos;
						D3DXVec3Normalize(&vPushDirA, &vPushDirA);
						vPushDirA.y = 0;
						//민다(Src만)
						pTransformA->AddForce(vPushDirA * fCollisionLength);

					}

				}

				//상대방이 죽은경우, 충돌처리를 하지 않는다.
				if (pSrc->Get_Dead())
					pDst->Erase_Collided(pSrc);
				if (pDst->Get_Dead())
					pSrc->Erase_Collided(pDst);
				if (pSrc->Get_Dead() || pDst->Get_Dead())
					continue;


				//만약 서로 충돌한 적이 없으면 서로의 충돌리스트에 서로를 추가한다.
				if (!pDst->Contain_Collided(pSrc))
				{
					pDst->Add_Collided(pSrc);
					pDst->Set_isCollided(true);
					pDst->OnCollisionEnter(pSrc);
				}
				if (!pSrc->Contain_Collided(pDst))
				{
					pSrc->Add_Collided(pDst);
					pSrc->Set_isCollided(true);
					pSrc->OnCollisionEnter(pDst);
				}


				pDst->OnCollisionStay(pSrc);
				pSrc->OnCollisionStay(pDst);
			}
			else
			{
				//만약 상대방이 전에 충돌한 리스트에 있으면 삭제한다.
				pDst->Erase_Collided(pSrc);
				pSrc->Erase_Collided(pDst);

				//충돌리스트가 비었고, 전에 충돌한적이 있으면 collisionExit을 실행한다.
				if (pDst->Get_CollidedSize() == 0 && pDst->Get_isCollided())
				{
					pDst->Set_isCollided(false);
					pDst->OnCollisionExit(pSrc);
				}
				if (pSrc->Get_CollidedSize() == 0 && pSrc->Get_isCollided())
				{
					pSrc->Set_isCollided(false);
					pSrc->OnCollisionExit(pDst);
				}
			}
		}

	}
	return S_OK;
}



void CCollisionMgr::Free()
{
	for (size_t i = 0; i < COL_END; ++i)
	{
		for (auto& pGameObject : m_CollisionGroup[i])
			Safe_Release(pGameObject);

		m_CollisionGroup[i].clear();
	}

}
