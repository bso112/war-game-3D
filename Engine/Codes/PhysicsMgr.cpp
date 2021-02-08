#include "..\Headers\PhysicsMgr.h"
#include "GameObject.h"
#include "Rigid_Body.h"
#include "Transform.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

CPhysicsMgr::CPhysicsMgr()
{

}

HRESULT CPhysicsMgr::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CPhysicsMgr::Ready_Component(void * pArg)
{
	return S_OK;
}

HRESULT CPhysicsMgr::Add_PhysGroup(PHYSGROUP eGroup, CTransform* pTransform, CCollider* pCollider, CRigid_Body* pRigid_Body, _float3* pNormal)
{
	PHYSSTRUCT* tagTemp = new PHYSSTRUCT;
	tagTemp->pTransform = pTransform;
	tagTemp->pCollider = pCollider;
	tagTemp->pRigidBody = pRigid_Body;
	tagTemp->pNormal = pNormal;
	m_PhysGroup[eGroup].push_back(tagTemp);

	return S_OK;
}

HRESULT CPhysicsMgr::Compute_PhysGroup(_double dblTimeDelta)
{
	for (auto& Group : m_PhysGroup)
	{
		for (auto Tag : Group)
		{
			Apply_Physics(dblTimeDelta, Tag);
		}
	}

	for (auto& Group : m_PhysGroup)
	{
		for (auto& Node : Group)
			Safe_Delete(Node);
		Group.clear();
	}

	return S_OK;
}

HRESULT CPhysicsMgr::Apply_Physics(_double dblTimeDelta, PHYSSTRUCT* tagPhys)
{
	//값 받아오기
	_float3 vVelocity = tagPhys->pRigidBody->Get_Velocity();
	_float3 vForce = tagPhys->pRigidBody->Get_ExternForce();
	CRigid_Body::STATEDESC	tStateDesc = tagPhys->pRigidBody->GetStateDesc();

	////속도값 힘으로 변환
	//vForce += vVelocity / dblTimeDelta * tagPhys->pRigidBody->GetStateDesc().fMass;


	//중력 작용
	vForce -= _float3(0.f, tagPhys->pRigidBody->GetStateDesc().fMass*m_fGravity*tagPhys->pRigidBody->GetStateDesc().fGravity_Multiply, 0.f);
	_bool bNormalIsV = 0;
	_float fFrictionVal = 0.f;
	_float3 vSlideNormal = _float3(0.f, 0.f, 0.f);
	//지면 충돌 관련 물리량vTempForce
	if (tagPhys->pNormal != nullptr)
	{
			//벡터 분해!(월드 수평, 월드 수직 성분)----------------------------------------------
			//수직방향 힘
		_float3 vVerForce = _float3(0.f, vForce.y, 0.f);

		//수평방향 법선벡터
		_float3 vHorNormal = *tagPhys->pNormal;
		vHorNormal.y = 0;
		D3DXVec3Normalize(&vHorNormal, &vHorNormal);

		//수평방향 힘
		_float fHorForce = D3DXVec3Dot(&vForce, &vHorNormal);
		_float3 vHorForce = vHorNormal * fHorForce;
		//if (fHorForce >= 0.f)
		//	vHorForce *= 0.f;
		if (fabs(D3DXVec3Length(&vHorForce)) < 0.01f)
			vHorForce *= 0.f;

		//나머지 힘
		_float3 vAbdtForce = vForce - vHorForce - vVerForce;
		if (vVerForce.y >= 0.f)
		{
			vAbdtForce += vVerForce;
			vVerForce *= 0.f;
		}


		//벡터 2차 분해(빗면방향, 빗면 수직방향) ----------------------------------------------
		//빗면방향 법선벡터 구하기
		_float3 vSlide = _float3(0.f, 0.f, 0.f);
		_float3 vDownward = _float3(0.f, 0.f, 0.f);

		_float3 vTempRight;
		D3DXVec3Cross(&vTempRight, &_float3(0.f, 1.f, 0.f), tagPhys->pNormal);
		D3DXVec3Normalize(&vTempRight, &vTempRight);

		tagPhys->pRigidBody->Set_NormalRight(vTempRight);

		_matrix matTemp;
		D3DXMatrixIdentity(&matTemp);
		D3DXMatrixRotationAxis(&matTemp, &vTempRight, D3DXToRadian(90.f));

		vSlideNormal = *tagPhys->pNormal; //빗면방향 법선벡터
		D3DXVec3TransformNormal(&vSlideNormal, &vSlideNormal, &matTemp);
		D3DXVec3Normalize(&vSlideNormal, &vSlideNormal);

		_float3 vInvNormal = -*tagPhys->pNormal;
		vSlide = vSlideNormal * (D3DXVec3Dot(&vSlideNormal, &vHorForce) + D3DXVec3Dot(&vSlideNormal, &vVerForce));
		vDownward = vInvNormal * (D3DXVec3Dot(&vInvNormal, &vHorForce) + D3DXVec3Dot(&vInvNormal, &vVerForce));

		tagPhys->pRigidBody->GetStateDesc_Direct().m_vSlideForce = vSlide;
		tagPhys->pRigidBody->GetStateDesc_Direct().m_vDownForce = vDownward;

		//////////////////////////////////
		//충돌에 따른 기존 속도 보정
		_float fVelNormal = D3DXVec3Dot(&vVelocity, tagPhys->pNormal);
		if (fVelNormal < 0.f)
		{
			_float3 vVelNormal = *tagPhys->pNormal * D3DXVec3Dot(tagPhys->pNormal, &vVelocity);
			vVelocity -= vVelNormal;
			tagPhys->pRigidBody->Set_Velocity(vVelocity);
		}

		//마찰력 계산
		fFrictionVal = 0.f;

		//정지마찰력or운동마찰력 선택
		if (D3DXVec3Length(&vVelocity) > 0.001f)
			fFrictionVal = tagPhys->pRigidBody->GetStateDesc().fFriction_Dynamic * tagPhys->pRigidBody->GetStateDesc().fMass * m_fGravity;
		else
			fFrictionVal = tagPhys->pRigidBody->GetStateDesc().fFriction_Static * tagPhys->pRigidBody->GetStateDesc().fMass * m_fGravity;

		vForce = vSlide + vAbdtForce;

		tagPhys->pRigidBody->Set_SlideNormal(vSlideNormal);
	}

	//vVelocity *= 0.f;

	//가속도 계산 및 적용
	_float3 Additional_Acc = vForce / tStateDesc.fMass;
	vVelocity += Additional_Acc*_float(dblTimeDelta);

	//종단속도 계산
	if (tStateDesc.fMaxFallSpeed != 0.f && tStateDesc.fMaxFallSpeed <= -vVelocity.y)
		vVelocity.y = -tStateDesc.fMaxFallSpeed;

	//마찰 있을 시 마찰력에 의한 속도감소
	if (tagPhys->pNormal != nullptr)
	{
		//if (bNormalIsV != 1)
		//{
		_float3 vSlideVel;
		_float3 vFrictionVelocity;
		_float	fFicVel = fFrictionVal / tagPhys->pRigidBody->GetStateDesc().fMass * dblTimeDelta;
		//vSlideVel = vSlideNormal * D3DXVec3Dot(&vVelocity, &vSlideNormal); //빗면방향으로의 속도 도출
		if (D3DXVec3Length(&vVelocity) <= fFicVel) //마찰력에 의한 속도감쇄가 빗면방향 기존 속도보다 클 경우
			vFrictionVelocity = -vVelocity;
		else
			vFrictionVelocity = -vVelocity * fFicVel / D3DXVec3Length(&vVelocity);

		vVelocity += vFrictionVelocity;
		tagPhys->pRigidBody->GetStateDesc_Direct().m_vInitForce = vFrictionVelocity;
	}

	if (D3DXVec3Length(&vVelocity) < 0.01f)
		vVelocity *= 0.f;

	//값 적용
	tagPhys->pRigidBody->Set_Velocity(vVelocity);

	//임시값 초기화
	tagPhys->pRigidBody->Reset_ExternForce();

	tagPhys->pTransform->ApplyPhysics(tagPhys->pRigidBody, dblTimeDelta);

	tagPhys->pRigidBody->GetStateDesc_Direct().m_vForce = vForce;

	return S_OK;
}

_float CPhysicsMgr::Compute_Optimal_Power(_float fAngle, _float3 vShooter, _float3 vTarget)
{
	_float fTempX = sqrtf(pow(vTarget.x - vShooter.x, 2.f) + pow(vTarget.z - vShooter.z, 2.f));
	_float fTempY = vTarget.y - vShooter.y;
	_float fRadian = D3DXToRadian(fAngle);

	//_float fTempC = 2 * pow(cosf(fRadian), 2.f)*(tanf(fRadian)*fTempX - fTempY);
	/////////////////////////////
	_float fTempC = tanf(fRadian)*fTempX - fTempY;

	if (fTempC <= 0.f)
		return -1;

	_float fPower = sqrtf(m_fGravity / (2.f*fTempC))*(fTempX / cosf(fRadian));

	///////////////////////////////////////////////////////////////
		//_float fVy = fPower*sinf(fRadian);
		//_float fVx = fPower*cosf(fRadian);
		//_float fTime = fTempX / fVx;
		//_float fY = fVy*fTime - m_fGravity*fTime*fTime / 2.f;

		//fPower = 100.f;

		//fVy = fPower*sinf(fRadian);
		//fVx = fPower*cosf(fRadian);
		//fTime = fTempX / fVx;
		//fY = fVy*fTime - m_fGravity*fTime*fTime / 2.f;
		///////////////////////////////////////////////////////////

		//if (tanf(fRadian)*fTempX <= fTempY)
		//	return -1;

		//_float fTempC = cosf(fRadian)*cosf(fRadian)*fTempY - cosf(fRadian)*sinf(fRadian)*fTempX;
		//if (fTempC >= 0.f)
		//	return -1;

		//_float fPower = fTempX*sqrtf(-m_fGravity / fTempC) / sqrtf(2.f);

	return fPower;
}

void CPhysicsMgr::Free()
{
	for (size_t i = 0; i < PHYS_END; ++i)
	{
		for (auto& pTag : m_PhysGroup[i])
		{
			Safe_Release(pTag->pTransform);
			Safe_Release(pTag->pCollider);
			Safe_Release(pTag->pRigidBody);
			Safe_Delete(pTag);
		}

		m_PhysGroup[i].clear();
	}
}
