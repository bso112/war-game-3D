#pragma once

#define RANDOM_INT(MIN, MAX) ((MIN) + rand() % ((MAX+1) - (MIN)))

#define ALPHATEST															\
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);			\
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);					\
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

#define ALPHATEST_END m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

#define ALPHABLEND															\
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		\
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	\
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

#define ALPHABLEND_END m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

