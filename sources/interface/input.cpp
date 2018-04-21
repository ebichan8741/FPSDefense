//=============================================================================
//
// ���͏��� [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "../main.h"
#include "input.h"
#include <stdio.h>

//=================================================================================================
// �}�N����`
//=================================================================================================
#define    COUNT_WAIT_REPEAT    (20)    // ���s�[�g�҂�����

//=================================================================================================
// �ÓI�ϐ�
//=================================================================================================
LPDIRECTINPUT8    CInput::m_pDInput = NULL;    // DirectInput�I�u�W�F�N�g

//*************************************************************************************************
// CInput�R���X�g���X�^
//*************************************************************************************************
CInput::CInput()
{
    m_pDIDevice = NULL;
}

//*************************************************************************************************
// CInput�f�X�g���X�^
//*************************************************************************************************
CInput::~CInput()
{
}

//*************************************************************************************************
// ���͏����̏���������
//*************************************************************************************************
HRESULT CInput::Init(HINSTANCE hInst, HWND hWnd)
{
    HRESULT hr = S_FALSE;

    // DirectInput�I�u�W�F�N�g�̍쐬
    if (!m_pDInput)
    {
        hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
            IID_IDirectInput8, (void**)&m_pDInput, NULL);
    }

    return hr;
}

//*************************************************************************************************
// ���͏����̍X�V����
//*************************************************************************************************
#if 0    // ---> �������z�֐���
HRESULT CInput::Update(void)
{
    return S_OK;
}
#endif

//*************************************************************************************************
// ���͏����̏I������
//*************************************************************************************************
void CInput::Uninit(void)
{
    if (m_pDIDevice)
    {// �f�o�C�X�I�u�W�F�N�g�̊J��
        m_pDIDevice->Unacquire();

        m_pDIDevice->Release();
        m_pDIDevice = NULL;
    }

    if (m_pDInput)
    {// DirectInput�I�u�W�F�N�g�̊J��
        m_pDInput->Release();
        m_pDInput = NULL;
    }
}

//*************************************************************************************************
// CInputKeyboard�R���X�g���X�^
//*************************************************************************************************
CInputKeyboard::CInputKeyboard()
{
    // �e���[�N�̃N���A
    ZeroMemory(m_aKeyState, sizeof m_aKeyState);
    ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
    ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
    ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
    ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//*************************************************************************************************
// CInputKeyboard�f�X�g���X�^
//*************************************************************************************************
CInputKeyboard::~CInputKeyboard()
{
}

//*************************************************************************************************
// �L�[�{�[�h�̏���������
//*************************************************************************************************
HRESULT CInputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
    HRESULT hr;

    // ���͏����̏�����
    CInput::Init(hInst, hWnd);

    // �f�o�C�X�I�u�W�F�N�g���쐬
    hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL);
    if (FAILED(hr) || m_pDIDevice == NULL)
    {
        MessageBox(hWnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
        return hr;
    }

    // �f�[�^�t�H�[�}�b�g��ݒ�
    hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
        return hr;
    }

    // �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
    hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
    if (FAILED(hr))
    {
        MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
        return hr;
    }

    // m_pDIDevice�A�N�Z�X�����l��(���͐���J�n)
    m_pDIDevice->Acquire();

    return hr;
}

//*************************************************************************************************
// �L�[�{�[�h���͏����̊J��
//*************************************************************************************************
void CInputKeyboard::Uninit(void)
{
    // ���͏����̊J��
    CInput::Uninit();
}

//*************************************************************************************************
// �L�[�{�[�h�̍X�V����
//*************************************************************************************************
void CInputKeyboard::Update(void)
{
    HRESULT hr;
    BYTE aKeyState[NUM_KEY_MAX];

    if (!m_pDIDevice)
    {
        return;
    }

    // �f�o�C�X����f�[�^���擾
    hr = m_pDIDevice->GetDeviceState(sizeof(aKeyState), aKeyState);
    if (SUCCEEDED(hr))
    {
        for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
        {
            m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];
            m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & ~aKeyState[nCntKey];
            m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

            if (aKeyState[nCntKey])
            {
                m_aKeyStateRepeatCnt[nCntKey]++;
                if (m_aKeyStateRepeatCnt[nCntKey] >= COUNT_WAIT_REPEAT)
                {
                    m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
                }
            }
            else
            {
                m_aKeyStateRepeatCnt[nCntKey] = 0;
                m_aKeyStateRepeat[nCntKey] = 0;
            }

            m_aKeyState[nCntKey] = aKeyState[nCntKey];
        }
    }
    else
    {
        m_pDIDevice->Acquire();
    }
}

//*************************************************************************************************
// �L�[�{�[�h�f�[�^�擾(�v���X)
//*************************************************************************************************
BOOL CInputKeyboard::GetKeyPress(int nKey)
{
    return (m_aKeyState[nKey] & 0x80) ? TRUE : FALSE;
}

//*************************************************************************************************
// �L�[�{�[�h�f�[�^�擾(�g���K�[)
//*************************************************************************************************
BOOL CInputKeyboard::GetKeyTrigger(int nKey)
{
    return (m_aKeyStateTrigger[nKey] & 0x80) ? TRUE : FALSE;
}

//*************************************************************************************************
// �L�[�{�[�h�f�[�^�擾(�����[�X)
//*************************************************************************************************
BOOL CInputKeyboard::GetKeyRelease(int nKey)
{
    return (m_aKeyStateRelease[nKey] & 0x80) ? TRUE : FALSE;
}

//*************************************************************************************************
// �L�[�{�[�h�f�[�^�擾(���s�[�g)
//*************************************************************************************************
BOOL CInputKeyboard::GetKeyRepeat(int nKey)
{
    return (m_aKeyStateRepeat[nKey] & 0x80) ? TRUE : FALSE;
}

//*************************************************************************************************
// �L�[�{�[�h�f�[�^�폜(�g���K�[)
//*************************************************************************************************
void CInputKeyboard::FlushKeyTrigger(int nKey)
{
    m_aKeyStateTrigger[nKey] = 0;
}

//-----------------------------------------------------------------------------
// �}�E�X
//-----------------------------------------------------------------------------
//*************************************************************************************************
// CInputMouse�R���X�g���X�^
//*************************************************************************************************
CInputMouse::CInputMouse()
{
    ZeroMemory(m_pMouseState.rgbButtons, sizeof m_pMouseState.rgbButtons);
    SetRect(&moveRect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);	    // �}�E�X�J�[�\���̓����͈�
    m_vMousePos.x = moveRect.left;	                        // �}�E�X�J�[�\���̂w���W��������
    m_vMousePos.y = moveRect.top;	                        // �}�E�X�J�[�\���̂x���W��������
    m_vMousePos.z = 0.0;
    lButton = false;	                        // ���{�^���̏���������
    rButton = false;	                        // �E�{�^���̏���������
    cButton = false;	                        // �����{�^���̏���������
    moveAdd = 1;	                            // �}�E�X�J�[�\���̈ړ��ʂ�ݒ�
}

//*************************************************************************************************
// CInputKeyboard�f�X�g���X�^
//*************************************************************************************************
CInputMouse::~CInputMouse()
{
}

//*************************************************************************************************
// �}�E�X�̏���������
//*************************************************************************************************
HRESULT CInputMouse::Init(HINSTANCE hInst, HWND hWnd)
{
    HRESULT hr;

    // ���͏����̏�����
    CInput::Init(hInst, hWnd);

    // �f�o�C�X�I�u�W�F�N�g���쐬
    hr = m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL);
    if (FAILED(hr) || m_pDIDevice == NULL)
    {
        MessageBox(hWnd, "�}�E�X���˂��I", "�x���I", MB_ICONWARNING);
        return hr;
    }

    // �f�[�^�t�H�[�}�b�g��ݒ�
    hr = m_pDIDevice->SetDataFormat(&c_dfDIMouse2);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "�}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
        return hr;
    }

    // �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
    hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_EXCLUSIVE));
    if (FAILED(hr))
    {
        MessageBox(hWnd, "�}�E�X�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
        return hr;
    }

    // �f�o�C�X�̐ݒ�
    DIPROPDWORD diprop;
    diprop.diph.dwSize = sizeof(diprop);
    diprop.diph.dwHeaderSize = sizeof(diprop.diph);
    diprop.diph.dwObj = 0;
    diprop.diph.dwHow = DIPH_DEVICE;
    diprop.dwData = DIPROPAXISMODE_REL;	// ���Βl���[�h�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j

    hr = m_pDIDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
    if (FAILED(hr)) {
        // �f�o�C�X�̐ݒ�Ɏ��s
        return false;
    }

    // m_pDIDevice�A�N�Z�X�����l��(���͐���J�n)
    m_pDIDevice->Acquire();

    return hr;
}

//*************************************************************************************************
// �}�E�X���͏����̊J��
//*************************************************************************************************
void CInputMouse::Uninit(void)
{
    // ���͏����̊J��
    CInput::Uninit();
}

//*************************************************************************************************
// �}�E�X�̍X�V����
//*************************************************************************************************
void CInputMouse::Update(void)
{
    HRESULT hr;

    if (!m_pDIDevice)
    {
        return;
    }

    m_pDIDevice->Acquire();

    hr = m_pDIDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_pMouseState);
    if (SUCCEEDED(hr))
    {
        // �m�F�p�̏����A�������� -->
        // �擾�����������Ƀ}�E�X�̏����X�V
        m_vMousePos.x += (m_pMouseState.lX * moveAdd);
        m_vMousePos.y += (m_pMouseState.lY * moveAdd);
        if (m_vMousePos.x < moveRect.left) m_vMousePos.x = moveRect.left;
        if (m_vMousePos.x > moveRect.right) m_vMousePos.x = moveRect.right;
        if (m_vMousePos.y < moveRect.top) m_vMousePos.y = moveRect.top;
        if (m_vMousePos.y > moveRect.bottom) m_vMousePos.y = moveRect.bottom;
        (m_pMouseState.rgbButtons[0] & 0x80) ? lButton = true : lButton = false;
        (m_pMouseState.rgbButtons[1] & 0x80) ? rButton = true : rButton = false;
        (m_pMouseState.rgbButtons[2] & 0x80) ? cButton = true : cButton = false;

        // �l���ς������\�����܂�
        char buf[128];
        wsprintf(buf, "(%5d, %5d, %5d) %s %s %s\n",
            (int)m_vMousePos.x, (int)m_vMousePos.y, m_pMouseState.lZ,
            GetMousePress(0) ? "Left" : "--",
            GetMousePress(1) ? "Right" : "--",
            GetMousePress(2) ? "Center" : "--");
        OutputDebugString(buf);
        // --> �����܂ŁA�m�F�p�̏���
    }
    else if (hr == DIERR_INPUTLOST)
    {
        m_pDIDevice->Acquire();
    }
}


void CInputMouse::GetMouseState(void)
{
#ifdef _DEBUG
    //   char buf[128];
    //wsprintf(buf, "(%5d, %5d, %5d) %s %s %s\n",
    //	m_pMouseState.lX, m_pMouseState.lY, m_pMouseState.lZ,
    //	(m_pMouseState.rgbButtons[0] & 0x80) ? "Left" : "--",
    //	(m_pMouseState.rgbButtons[1] & 0x80) ? "Right" : "--",
    //	(m_pMouseState.rgbButtons[2] & 0x80) ? "Center" : "--");
    //OutputDebugString(buf);
#endif

}

//*************************************************************************************************
// �}�E�X�f�[�^�擾(�v���X)
//*************************************************************************************************
bool CInputMouse::GetMousePress(int nClick)
{
    return (m_pMouseState.rgbButtons[nClick] & 0x80) ? true : false;
}

//*************************************************************************************************
// �}�E�X�f�[�^�擾(�g���K�[)
//*************************************************************************************************
bool CInputMouse::GetMouseTrigger(int nClick)
{
    // �O�Ɏ擾�������̃}�E�X�̏��
    static bool prevState[sizeof(m_pMouseState.rgbButtons) / sizeof(m_pMouseState.rgbButtons[nClick])];

    //���̃}�E�X�̏�� 
    bool current = m_pMouseState.rgbButtons[nClick] & 0x80;

    //�O�̏�Ԃ�false�ŁA���̏�Ԃ�true�Ȃ�΁A�N���b�N�����u�ԂƔ��肷�� 
    bool ret = current && !prevState[nClick];

    //���̏�Ԃ�ۑ����� 
    prevState[nClick] = current;

    //���茋�ʂ�Ԃ� 
    return ret;
}

//*************************************************************************************************
// �}�E�X���W�ݒ�
//*************************************************************************************************
void CInputMouse::SetMousePos(D3DXVECTOR3 vPos)
{
    m_vMousePos = vPos;
}

//*************************************************************************************************
// �}�E�X���W�擾
//*************************************************************************************************
D3DXVECTOR3 CInputMouse::GetMousePos(void)
{
    return m_vMousePos;
}