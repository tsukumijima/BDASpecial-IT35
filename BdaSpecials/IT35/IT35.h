#pragma once

#include "IBdaSpecials2.h"

class CIT35Specials : public IBdaSpecials2b5
{
public:
	CIT35Specials(CComPtr<IBaseFilter> pTunerDevice, const WCHAR* szTunerDisplayName);
	virtual ~CIT35Specials(void);

	const HRESULT InitializeHook(void);

	const HRESULT LockChannel(const TuningParam *pTuningParam);
	const HRESULT ReadIniFile(const WCHAR *szIniFilePath);
	const HRESULT PreLockChannel(TuningParam *pTuningParam);
	const HRESULT PreTuneRequest(const TuningParam *pTuningParam, ITuneRequest *pITuneRequest);
	const HRESULT PostTuneRequest(const TuningParam *pTuningParam);

	virtual void Release(void);

	static HMODULE m_hMySelf;

private:
	CComPtr<IBaseFilter> m_pTunerDevice;
	CComQIPtr<IKsPropertySet> m_pIKsPropertySet;
	CComPtr<IBDA_FrequencyFilter> m_pIBDA_FrequencyFilter;				// IBDA_FrequencyFilter (Input Pin, Node 0)
	CComPtr<IBDA_SignalStatistics> m_pIBDA_SignalStatistics;			// IBDA_SignalStatistics(Input Pin, Node 0)
	CComPtr<IBDA_LNBInfo> m_pIBDA_LNBInfo;								// IBDA_LNBInfo (Input Pin, Node 0)
	CComPtr<IBDA_DigitalDemodulator> m_pIBDA_DigitalDemodulator;		// IBDA_DigitalDemodulator (Output Pin, Node 1)
	CComPtr<IBDA_DeviceControl> m_pIBDA_DeviceControl;					// IBDA_DeviceControl (Tuner)
	CRITICAL_SECTION m_CriticalSection;
	HANDLE m_hSemaphore;												// �v���Z�X�Ԕr���p
	std::wstring m_sTunerGUID;											// Tuner��GUID

	// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	enum enumPrivateSetTSID {
		ePrivateSetTSIDNone = 0,				// �s��Ȃ�
		ePrivateSetTSIDPreTR,					// PreTuneRequest�ōs��
		ePrivateSetTSIDPostTR,					// PostTuneRequest�ōs��
		ePrivateSetTSIDSpecial = 100,			// �S�Ẵ`���[�j���O�����LockChannel�ōs��
	};

	// USB Device Vender ID & Product ID
	enum enumProductID {
		PID_UNKNOWN =	-1,
		PID_W3U4 =		0x0511083f,		// PLEX PX-W3U4
		PID_Q3U4 =		0x0511084a,		// PLEX PX-Q3U4
		PID_W3PE4 =		0x0511023f,		// PLEX	PX-W3PE4
		PID_W3PE5 =		0x0511073f,		// PLEX	PX-W3PE5
		PID_Q3PE4 =		0x0511024a,		// PLEX PX-Q3PE4
		PID_Q3PE5 =		0x0511074a,		// PLEX PX-Q3PE5
		PID_1T1S =		0x0511004b,		// e-better DTV02-1T1S-U
		PID_5T =		0x0511024d,		// e-better DTV02-5T-P
		PID_4TS =		0x05110254,		// e-better DTV02A-4TS-P
		PID_MLT5PE =	0x0511024e,		// PLEX PX-MLT5PE
		PID_MLT8PE3 =	0x05110252,		// PLEX PX-MLT8PE3
		PID_MLT8PE5 =	0x05110253,		// PLEX PX-MLT8PE5
	};

	// IT9305E �R�}���h
	enum USB_CMD {
		CMD_MEM_RD = 0x00,
		CMD_MEM_WR = 0x01,
		CMD_I2C_RD = 0x02,
		CMD_I2C_WR = 0x03,
		CMD_IR_GET = 0x18,
		CMD_FW_DL = 0x21,
		CMD_FW_QUERYINFO = 0x22,
		CMD_FW_BOOT = 0x23,
		CMD_FW_DL_BEGIN = 0x24,
		CMD_FW_DL_END = 0x25,
		CMD_FW_SCATTER_WR = 0x29,
		CMD_GENERIC_I2C_RD = 0x2a,
		CMD_GENERIC_I2C_WR = 0x2b,
		CMD_1023 = 0x1023,
	};

	enum I2C_MASTER_RW {
		I2C_MASTER_WRITE = 0x00,
		I2C_MASTER_READ = 0x01,
	};

	struct i2c_info {
		BYTE bus;
		BYTE addr;
	};

	struct i2c_thru_info {
		i2c_info Parent;
		BYTE child;
	};

	struct i2c_slaves_cxd2856 {
		i2c_info slvt;
		i2c_info slvx;
		i2c_info gate;
	};

	// PX-MLT5PE��I2C�o�XNo./�A�h���X�̃��X�g
	i2c_slaves_cxd2856 m_aI2c_slaves_mlt5pe[5] = {
		{{0x03, 0x65}, {0x03, 0x67}, {0x03, 0x60}},		// tuner 0
		{{0x01, 0x6c}, {0x01, 0x6e}, {0x01, 0x60}},		// tuner 1
		{{0x01, 0x64}, {0x01, 0x66}, {0x01, 0x60}},		// tuner 2
		{{0x03, 0x6c}, {0x03, 0x6e}, {0x03, 0x60}},		// tuner 3
		{{0x03, 0x64}, {0x03, 0x66}, {0x03, 0x60}},		// tuner 4
	};

	enumProductID m_ProductID;					// USB VID & PID
	DWORD m_nTunerID;							// Tuner�ԍ�
	BOOL m_bRewriteIFFreq;						// IF���g���� put_CarrierFrequency() ���s��
	enumPrivateSetTSID m_nPrivateSetTSID;		// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	BOOL m_bLNBPowerON;							// LNB�d���̋�����ON����
	BOOL m_bDualModeISDB;						// Dual Mode ISDB Tuner
	unsigned int m_nSpecialLockConfirmTime;		// BDASpecial�ŗL��LockChannel���g�p����ꍇ��ISDB-S Lock�����m�F����
	unsigned int m_nSpecialLockSetTSIDInterval;	// BDASpecial�ŗL��LockChannel���g�p����ꍇ��ISDB-S Lock�����҂�����TSID�̍ăZ�b�g���s���C���^�[�o������
	BOOL m_bRewriteNominalRate;					// ISDB-T���ACXD2856��Nominal Rate���Đݒ肷��
	BYTE m_byNominalRate_List[5];				// ISDB-T���ACXD2856�ɐݒ肷��Nominal Rate
	DWORD m_nLastIsdbMode;						// ���݂�ISDB���[�h
	ModulationType m_nLastModulationType;		// ���݂̕ϒ�����
	ULONG m_nLastMultiplier;					// ���݂̎��g�����{
	ULONG m_nLastBandWidth;						// ���݂̑ш敝

private:
	WORD it35_checksum(const BYTE* buf, size_t len);
	void it35_create_msg(WORD cmd, const BYTE* wbuf, DWORD wlen, BYTE* msg, DWORD* mlen);
	int it35_tx_bulk_msg(WORD cmd, const BYTE* wbuf, DWORD wlen, BYTE* rbuf, DWORD* rlen);
	int it35_i2c_wr(BYTE i2c_bus, BYTE i2c_addr, BYTE reg, BYTE* data, DWORD len);
	int it35_i2c_rd(BYTE i2c_bus, BYTE i2c_addr, BYTE* data, DWORD* len);
	int it35_mem_wr_regs(DWORD reg, BYTE* data, DWORD len);
	int it35_mem_wr_reg(DWORD reg, BYTE data);
	int it35_mem_rd_regs(DWORD reg, BYTE* data, DWORD* len);
	int it35_mem_rd_reg(DWORD reg, BYTE* data);
	int it35_mem_set_reg_bits(DWORD reg, BYTE data, BYTE mask);
	int it35_i2c_wr_regs(i2c_info slave, BYTE reg, BYTE* data, DWORD len);
	int it35_i2c_wr_reg(i2c_info slave, BYTE reg, BYTE data);
	int it35_i2c_rd_regs(i2c_info slave, BYTE reg, BYTE* data, DWORD len);
	int it35_i2c_rd_reg(i2c_info slave, BYTE reg, BYTE* data);
	int it35_i2c_set_reg_bits(i2c_info slave, BYTE reg, BYTE data, BYTE mask);
	int it35_i2c_thru_wr_regs(i2c_thru_info slaves, BYTE reg, BYTE* data, DWORD len);
	int it35_i2c_thru_wr_reg(i2c_thru_info slaves, BYTE reg, BYTE data);
	int it35_i2c_thru_rd_regs(i2c_thru_info slaves, BYTE reg, BYTE* data, DWORD len);
	int it35_i2c_thru_rd_reg(i2c_thru_info slaves, BYTE reg, BYTE* data);
	int it35_i2c_thru_set_reg_bits(i2c_thru_info slaves, BYTE reg, BYTE data, BYTE mask);

	static inline BYTE I2C_ADDR_DATA(BYTE addr, I2C_MASTER_RW rw)
	{
		return (BYTE)((addr << 1) | rw);
	}
};

class LockProc {
private:
	HANDLE* pSemaphore;
	DWORD result;

public:
	LockProc(HANDLE* pHandle, DWORD dwMilliSeconds);
	LockProc(HANDLE* pHandle);
	~LockProc(void);
	BOOL IsSuccess(void);
};
