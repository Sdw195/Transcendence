//	BackgroundTasks.h
//
//	Transcendence background tasks
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

class CChangePasswordTask : public IHITask
	{
	public:
		CChangePasswordTask (CHumanInterface &HI, CCloudService &Service, const CString &sUsername, const CString &sOldPassword, const CString &sNewPassword) : IHITask(HI), m_Service(Service), m_sUsername(sUsername), m_sOldPassword(sOldPassword), m_sNewPassword(sNewPassword) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.ChangePassword(pProcessor, m_sUsername, m_sOldPassword, m_sNewPassword, retsResult); }

	private:
		CCloudService &m_Service;
		CString m_sUsername;
		CString m_sOldPassword;
		CString m_sNewPassword;
	};

class CInitAdventureTask : public IHITask
	{
	public:
		CInitAdventureTask (CHumanInterface &HI, CTranscendenceModel &Model, DWORD dwAdventure) : IHITask(HI), m_Model(Model), m_dwAdventure(dwAdventure) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Model.InitAdventure(m_dwAdventure, retsResult); }

	private:
		CTranscendenceModel &m_Model;
		DWORD m_dwAdventure;
	};

class CInitModelTask : public IHITask
	{
	public:
		CInitModelTask (CHumanInterface &HI, CTranscendenceModel &Model) : IHITask(HI), m_Model(Model) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Model.InitBackground(retsResult); }

	private:
		CTranscendenceModel &m_Model;
	};

class CLoadGameTask : public IHITask
	{
	public:
		CLoadGameTask (CHumanInterface &HI, CTranscendenceModel &Model, const CString &sUsername, const CString &sFilespec) : IHITask(HI), m_Model(Model), m_sUsername(sUsername), m_sFilespec(sFilespec) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Model.LoadGame(m_sUsername, m_sFilespec, retsResult); }

	private:
		CTranscendenceModel &m_Model;
		CString m_sUsername;
		CString m_sFilespec;
	};

class CLoadGameWithSignInTask : public IHITask
	{
	public:
		CLoadGameWithSignInTask (CHumanInterface &HI, CCloudService &Service, CTranscendenceModel &Model, const CString &sFilespec) : IHITask(HI), m_Service(Service), m_Model(Model), m_sFilespec(sFilespec) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult);

	private:
		CCloudService &m_Service;
		CTranscendenceModel &m_Model;
		CString m_sFilespec;
	};

class CPostRecordTask : public IHITask
	{
	public:
		CPostRecordTask (CHumanInterface &HI, CCloudService &Service, const CGameRecord &Record, const CGameStats &Stats) : IHITask(HI), m_Service(Service), m_Record(Record), m_Stats(Stats) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.PostGameRecord(pProcessor, m_Record, m_Stats, retsResult); }

	private:
		CCloudService &m_Service;
		CGameRecord m_Record;
		CGameStats m_Stats;
	};

class CPostStatsTask : public IHITask
	{
	public:
		CPostStatsTask (CHumanInterface &HI, CCloudService &Service, const CGameStats &Stats) : IHITask(HI), m_Service(Service), m_Stats(Stats) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.PostGameStats(pProcessor, m_Stats, retsResult); }

	private:
		CCloudService &m_Service;
		CGameStats m_Stats;
	};

class CRegisterUserTask : public IHITask
	{
	public:
		CRegisterUserTask (CHumanInterface &HI, CCloudService &Service, const CString &sUsername, const CString &sPassword, const CString &sEmail, bool bAutoSignIn) : IHITask(HI), m_Service(Service), m_sUsername(sUsername), m_sPassword(sPassword), m_sEmail(sEmail), m_bAutoSignIn(bAutoSignIn) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.RegisterUser(pProcessor, m_sUsername, m_sPassword, m_sEmail, m_bAutoSignIn, retsResult); }

	private:
		CCloudService &m_Service;
		CString m_sUsername;
		CString m_sPassword;
		CString m_sEmail;
		bool m_bAutoSignIn;
	};

class CServiceHousekeepingTask : public IHITask
	{
	public:
		CServiceHousekeepingTask (CHumanInterface &HI, CCloudService &Service) : IHITask(HI), m_Service(Service) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.Housekeeping(pProcessor); }

	private:
		CCloudService &m_Service;
	};

class CSignInUserTask : public IHITask
	{
	public:
		CSignInUserTask (CHumanInterface &HI, CCloudService &Service, const CString &sUsername, const CString &sPassword, bool bAutoSignIn) : IHITask(HI), m_Service(Service), m_sUsername(sUsername), m_sPassword(sPassword), m_bAutoSignIn(bAutoSignIn) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.SignInUser(pProcessor, m_sUsername, m_sPassword, m_bAutoSignIn, retsResult); }

	private:
		CCloudService &m_Service;
		CString m_sUsername;
		CString m_sPassword;
		bool m_bAutoSignIn;
	};

class CSignOutUserTask : public IHITask
	{
	public:
		CSignOutUserTask (CHumanInterface &HI, CCloudService &Service) : IHITask(HI), m_Service(Service) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Service.SignOutUser(pProcessor, retsResult); }

	private:
		CCloudService &m_Service;
	};

class CStartNewGameTask : public IHITask
	{
	public:
		CStartNewGameTask (CHumanInterface &HI, CTranscendenceModel &Model) : IHITask(HI), m_Model(Model) { }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult) { return m_Model.StartNewGameBackground(retsResult); }

	private:
		CTranscendenceModel &m_Model;
	};