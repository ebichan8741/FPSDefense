//=============================================================================
//
// �Q�[�����[�h�Ǘ� [GameMode.h]
// Author : TAKUYA EBIHARA
//
//=============================================================================
#ifndef _GAMEMODE_H_
#define _GAMEMODE_H_

#include <vector>
#include "../manager/modeManager.h"

#define NUMBER_DIGIT	(5)		// �e���̌�
#define ENEMY_NUM		(1)		// �G�l�~�[�̐�
#define HP_DIGIT		(3)		// HP�̌�

//=============================================================================
//�O���錾
//=============================================================================
//class CMeshField;
//CMeshField *GetMeshField(void);
class CPlayer;
CPlayer *GetPlayer(void);
//class CEnemy;
//CEnemy *GetEnemy(int ID);
//class CNumber;
//CNumber *GetBulletNumber(int ID);
//CNumber *GetHPNumber(int ID);

//=============================================================================
// �N���X��`
//=============================================================================
class CGameMode :public CMode
{
public:
    HRESULT Init(void);
    void Uninit(void);
    void Update(void);
    void Draw(void);

    //static CMeshField *GetMeshField(void);
    static CPlayer *GetPlayer(void);
    //static CEnemy *GetEnemy(int ID);
    //static CNumber *GetBulletNumber(int ID);
    //static CNumber *GetHPNumber(int ID);

    static bool m_flag;
    static int m_EnemyCnt;

private:
    static CPlayer *m_pPlayer;
    //static CMeshField *m_pMeshField;
    //static CEnemy *m_pEnemy[ENEMY_NUM];
    //static CNumber *m_pBulletNumber[NUMBER_DIGIT];
    //static CNumber *m_pHPNumber[HP_DIGIT];

};

#endif