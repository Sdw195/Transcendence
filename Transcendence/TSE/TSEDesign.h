//	TSEDesign.h
//
//	Transcendence design classes

#ifndef INCL_TSE_DESIGN
#define INCL_TSE_DESIGN

class CCommunicationsHandler;
class CDockScreen;
class CGameStats;
class CItemCtx;
class CObjectImageArray;
class CShipClass;
class CSystemMap;
class CTopology;
class CTopologyDescTable;
class IDeviceGenerator;
struct SDestroyCtx;
struct SSystemCreateCtx;

//	Constants & Enums

const int MAX_OBJECT_LEVEL =			25;	//	Max level for space objects
const int MAX_ITEM_LEVEL =				25;	//	Max level for items

enum ItemCategories
	{
	itemcatMisc =			0x00000001,		//	General item with no object UNID
	itemcatArmor =			0x00000002,		//	Armor items
	itemcatWeapon =			0x00000004,		//	Primary weapons
	itemcatMiscDevice =		0x00000008,		//	Misc device
	itemcatLauncher =		0x00000010,		//	Launcher
	itemcatNano =			0x00000020,
	itemcatReactor =		0x00000040,		//	Reactor device
	itemcatShields =		0x00000080,		//	Shield device
	itemcatCargoHold =		0x00000100,		//	Cargo expansion device
	itemcatFuel =			0x00000200,		//	Fuel items
	itemcatMissile =		0x00000400,		//	Missiles
	itemcatDrive =			0x00000800,		//	Drive device
	itemcatUseful =			0x00001000,		//	Usable item

	itemcatWeaponMask =		0x00000014,		//	Either primary or launcher
	itemcatDeviceMask =		0x000009DC,		//	Any device

	itemcatCount =			13,				//	Total count
	};

//	CFormulaText

class CFormulaText
	{
	public:
		int EvalAsInteger (CSpaceObject *pSource, CString *retsPrefix = NULL, CString *retsSuffix = NULL, CString *retsError = NULL) const;
		ALERROR InitFromString (SDesignLoadCtx &Ctx, const CString &sText);
		inline bool IsEmpty (void) const { return m_sText.IsBlank(); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetInteger (int iValue) { m_sText = strFromInt(iValue); }
		void WriteToStream (IWriteStream *pStream) const;

	private:
		CString m_sText;
	};

//	Base Design Type ----------------------------------------------------------
//
//	To add a new DesignType:
//
//	[ ]	Add it to DesignTypes enum
//	[ ] Increment designCount
//	[ ] Add a char??? entry
//	[ ] Add entry to DESIGN_CHAR in CDesignType (make sure it matches the char??? entry)
//	[ ] Add case to CDesignTypeCriteria::ParseCriteria (if type should be enumerable)

enum DesignTypes
	{
	designItemType =					0,
	designItemTable =					1,
	designShipClass =					2,
	designEnergyFieldType =				3,
	designSystemType =					4,
	designStationType =					5,
	designSovereign =					6,
	designDockScreen =					7,
	designEffectType =					8,
	designPower =						9,

	designSpaceEnvironmentType =		10,
	designShipTable =					11,
	designAdventureDesc =				12,
	designGlobals =						13,
	designImage =						14,
	designSound =						15,
	designSystemNode =					16,
	designSystemTable =					17,
	designSystemMap =					18,
	designNameGenerator =				19,

	designEconomyType =					20,
	designTemplateType =				21,

	designCount	=						22, 

	designSetAll =						0xffffffff,
	charEconomyType =					'$',
	charAdventureDesc =					'a',
	charItemTable =						'b',
	charEffectType =					'c',
	charDockScreen =					'd',
	charSpaceEnvironmentType =			'e',
	charEnergyFieldType =				'f',
	charGlobals =						'g',
	charShipTable =						'h',
	charItemType =						'i',

	charImage =							'm',
	charSystemNode =					'n',
	charPower =							'p',
	charSystemTable =					'q',
	charShipClass =						's',
	charStationType =					't',
	charSound =							'u',
	charSovereign =						'v',
	charNameGenerator =					'w',
	charSystemType =					'y',

	charSystemMap =						'z',
	charTemplateType =					'_',
	};

class CDesignTypeCriteria
	{
	public:
		inline bool ChecksLevel (void) const { return (m_iGreaterThanLevel != INVALID_COMPARE || m_iLessThanLevel != INVALID_COMPARE); }
		inline const CString &GetExcludedAttrib (int iIndex) const { return m_sExclude[iIndex]; }
		inline int GetExcludedAttribCount (void) const { return m_sExclude.GetCount(); }
		inline const CString &GetExcludedSpecialAttrib (int iIndex) const { return m_sExcludeSpecial[iIndex]; }
		inline int GetExcludedSpecialAttribCount (void) const { return m_sExcludeSpecial.GetCount(); }
		inline const CString &GetRequiredAttrib (int iIndex) const { return m_sRequire[iIndex]; }
		inline int GetRequiredAttribCount (void) const { return m_sRequire.GetCount(); }
		inline const CString &GetRequiredSpecialAttrib (int iIndex) const { return m_sRequireSpecial[iIndex]; }
		inline int GetRequiredSpecialAttribCount (void) const { return m_sRequireSpecial.GetCount(); }
		inline bool IncludesVirtual (void) const { return m_bIncludeVirtual; }
		inline bool MatchesDesignType (DesignTypes iType) const
			{ return ((m_dwTypeSet & (1 << iType)) ? true : false); }
		bool MatchesLevel (int iLevel) const;
		static ALERROR ParseCriteria (const CString &sCriteria, CDesignTypeCriteria *retCriteria);

	private:
		enum Flags
			{
			INVALID_COMPARE = -1000,
			};

		DWORD m_dwTypeSet;
		TArray<CString> m_sRequire;
		TArray<CString> m_sExclude;
		TArray<CString> m_sRequireSpecial;
		TArray<CString> m_sExcludeSpecial;

		int m_iGreaterThanLevel;
		int m_iLessThanLevel;

		bool m_bIncludeVirtual;
	};

class CDesignType
	{
	public:
		enum ECachedHandlers
			{
			evtOnGlobalTypesInit		= 0,
			evtOnObjDestroyed			= 1,
			evtOnSystemObjAttacked		= 2,
			evtOnSystemWeaponFire		= 3,

			evtCount					= 4,
			};

		CDesignType (void);
		virtual ~CDesignType (void);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CDesignType **retpType);

		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError);
		inline ALERROR FinishBindDesign (SDesignLoadCtx &Ctx) { return OnFinishBindDesign(Ctx); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		bool MatchesCriteria (const CDesignTypeCriteria &Criteria);
		ALERROR PrepareBindDesign (SDesignLoadCtx &Ctx);
		inline void PrepareReinit (void) { OnPrepareReinit(); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Reinit (void);
		inline void UnbindDesign (void) { OnUnbindDesign(); }
		void WriteToStream (IWriteStream *pStream);

		static CDesignType *AsType (CDesignType *pType) { return pType; }
		inline CEffectCreator *FindEffectCreatorInType (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		bool FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const;
		inline bool FindEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_EventsCache[iEvent]; return (m_EventsCache[iEvent].pCode != NULL); }
		bool FindStaticData (const CString &sAttrib, const CString **retpData) const;
		void FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent = eventNone, ICCItem **retpResult = NULL);
		bool FireGetCreatePos (CSpaceObject *pBase, CSpaceObject *pTarget, CSpaceObject **retpGate, CVector *retvPos);
		void FireGetGlobalAchievements (CGameStats &Stats);
		bool FireGetGlobalDockScreen (const SEventHandlerDesc &Event, CSpaceObject *pObj, CString *retsScreen, int *retiPriority);
		int FireGetGlobalResurrectPotential (void);
		void FireObjCustomEvent (const CString &sEvent, CSpaceObject *pObj, ICCItem **retpResult);
		ALERROR FireOnGlobalDockPaneInit (const SEventHandlerDesc &Event, void *pScreen, DWORD dwScreenUNID, const CString &sScreen, const CString &sPane, CString *retsError);
		void FireOnGlobalObjDestroyed (const SEventHandlerDesc &Event, SDestroyCtx &Ctx);
		ALERROR FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip, CString *retsError = NULL);
		ALERROR FireOnGlobalPlayerEnteredSystem (CString *retsError = NULL);
		ALERROR FireOnGlobalPlayerLeftSystem (CString *retsError = NULL);
		ALERROR FireOnGlobalResurrect (CString *retsError = NULL);
		ALERROR FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx, CString *retsError = NULL);
		ALERROR FireOnGlobalTopologyCreated (CString *retsError = NULL);
		ALERROR FireOnGlobalTypesInit (SDesignLoadCtx &Ctx);
		ALERROR FireOnGlobalUniverseCreated (const SEventHandlerDesc &Event);
		ALERROR FireOnGlobalUniverseLoad (const SEventHandlerDesc &Event);
		ALERROR FireOnGlobalUniverseSave (const SEventHandlerDesc &Event);
		void FireOnRandomEncounter (CSpaceObject *pObj = NULL);
		inline const CString &GetAttributes (void) { return m_sAttributes; }
		inline CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		inline int GetDataFieldInteger (const CString &sField) { CString sValue; if (FindDataField(sField, &sValue)) return strToInt(sValue, 0, NULL); else return 0; }
		ICCItem *GetEventHandler (const CString &sEvent) const;
		void GetEventHandlers (const CEventHandler **retHandlers, TSortMap<CString, SEventHandlerDesc> *retInheritedHandlers);
		SExtensionDesc *GetExtension (void) const { return m_pExtension; }
		inline const CString &GetGlobalData (const CString &sAttrib) { return m_GlobalData.GetData(sAttrib); }
		inline CDesignType *GetInheritFrom (void) const { return m_pInheritFrom; }
		inline CXMLElement *GetLocalScreens (void) const { return m_pLocalScreens; }
		CXMLElement *GetScreen (const CString &sUNID);
		const CString &GetStaticData (const CString &sAttrib) const;
		inline CString GetTypeName (void) { return GetDataField(CONSTLIT("name")); }
		inline DWORD GetUNID (void) const { return m_dwUNID; }
		inline DWORD GetVersion (void) const { return m_dwVersion; }
		inline bool HasAttribute (const CString &sAttrib) const { return ::HasModifier(m_sAttributes, sAttrib); }
		inline bool HasEvents (void) const { return !m_Events.IsEmpty() || (m_pInheritFrom && m_pInheritFrom->HasEvents()); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		void InitCachedEvents (int iCount, char **pszEvents, SEventHandlerDesc *retEvents);
		inline void SetGlobalData (const CString &sAttrib, const CString &sData) { m_GlobalData.SetData(sAttrib, sData); }
		inline void SetUNID (DWORD dwUNID) { m_dwUNID = dwUNID; }
		inline bool TranslateText (CSpaceObject *pObj, const CString &sID, CString *retsText) { return m_Language.Translate(pObj, sID, retsText); }

		static CString GetTypeChar (DesignTypes iType);

		//	CDesignType overrides
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual CCommunicationsHandler *GetCommsHandler (void) { return NULL; }
		virtual int GetLevel (void) const { return -1; }
		virtual DesignTypes GetType (void) = 0;
		virtual bool IsVirtual (void) const { return false; }

	protected:
		ALERROR AddEventHandler (const CString &sEvent, const CString &sCode, CString *retsError = NULL) { return m_Events.AddEvent(sEvent, sCode, retsError); }
		void ReadGlobalData (SUniverseLoadCtx &Ctx);
		void ReportEventError (const CString &sEvent, ICCItem *pError);

		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const { return false; }
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual void OnPrepareReinit (void) { }
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) { }
		virtual void OnReinit (void) { }
		virtual void OnUnbindDesign (void) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }

	private:
		void AddUniqueHandlers (TSortMap<CString, SEventHandlerDesc> *retInheritedHandlers);
		inline SEventHandlerDesc *GetInheritedCachedEvent (ECachedHandlers iEvent) { return (m_EventsCache[iEvent].pCode ? &m_EventsCache[iEvent] : (m_pInheritFrom ? m_pInheritFrom->GetInheritedCachedEvent(iEvent) : NULL)); }
		void InitCachedEvents (void);
		bool InSelfReference (CDesignType *pType);
		void MergeLanguageTo (CLanguageDataBlock &Dest);

		DWORD m_dwUNID;
		SExtensionDesc *m_pExtension;			//	Extension
		DWORD m_dwVersion;						//	Extension version

		DWORD m_dwInheritFrom;					//	Inherit from this type
		CDesignType *m_pInheritFrom;			//	Inherit from this type

		CString m_sAttributes;					//	Type attributes
		CAttributeDataBlock m_StaticData;		//	Static data
		CAttributeDataBlock m_GlobalData;		//	Global (variable) data
		CAttributeDataBlock m_InitGlobalData;	//	Initial global data
		CLanguageDataBlock m_Language;			//	Language data
		CEventHandler m_Events;					//	Event handlers
		CXMLElement *m_pLocalScreens;			//	Local dock screen

		SEventHandlerDesc m_EventsCache[evtCount];	//	Cached events
	};

template <class CLASS> class CDesignTypeRef
	{
	public:
		CDesignTypeRef (void) : m_pType(NULL), m_dwUNID(0) { }

		inline operator CLASS *() const { return m_pType; }
		inline CLASS * operator->() const { return m_pType; }

		ALERROR Bind (SDesignLoadCtx &Ctx)
			{
			if (m_dwUNID)
				{
				CDesignType *pBaseType = g_pUniverse->FindDesignType(m_dwUNID);
				if (pBaseType == NULL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Unknown design type: %x"), m_dwUNID);
					return ERR_FAIL;
					}

				m_pType = CLASS::AsType(pBaseType);
				if (m_pType == NULL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Specified type is invalid: %x"), m_dwUNID);
					return ERR_FAIL;
					}
				}

			return NOERROR;
			}

		inline DWORD GetUNID (void) const { return m_dwUNID; }
		void LoadUNID (SDesignLoadCtx &Ctx, const CString &sUNID) { m_dwUNID = ::LoadUNID(Ctx, sUNID); }

		void Set (CLASS *pType)
			{
			m_pType = pType;
			if (pType)
				m_dwUNID = pType->GetUNID();
			else
				m_dwUNID = 0;
			}

	protected:
		CLASS *m_pType;
		DWORD m_dwUNID;
	};

//	Design Type References ----------------------------------------------------

class CItemTypeRef : public CDesignTypeRef<CItemType>
	{
	public:
		CItemTypeRef (void) { }
		CItemTypeRef (CItemType *pType) { Set(pType); }
		inline ALERROR Bind (SDesignLoadCtx &Ctx) { return CDesignTypeRef<CItemType>::Bind(Ctx); }
		ALERROR Bind (SDesignLoadCtx &Ctx, ItemCategories iCategory);
	};

class CArmorClassRef : public CDesignTypeRef<CArmorClass>
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
	};

class CDeviceClassRef : public CDesignTypeRef<CDeviceClass>
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void Set (CDeviceClass *pDevice);
	};

class CWeaponFireDescRef : public CDesignTypeRef<CWeaponFireDesc>
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
	};

class CDockScreenTypeRef
	{
	public:
		CDockScreenTypeRef (void) : m_pType(NULL), m_pLocal(NULL) { }

		inline operator CDockScreenType *() const { return m_pType; }
		inline CDockScreenType * operator->() const { return m_pType; }

		ALERROR Bind (SDesignLoadCtx &Ctx, CXMLElement *pLocalScreens = NULL);
		CXMLElement *GetDesc (void) const;
		CDesignType *GetDockScreen (CDesignType *pRoot, CString *retsName) const;
		CString GetStringUNID (CDesignType *pRoot) const;
		inline const CString &GetUNID (void) const { return m_sUNID; }
		void LoadUNID (SDesignLoadCtx &Ctx, const CString &sUNID);

		ALERROR Bind (CXMLElement *pLocalScreens = NULL);
		inline void LoadUNID (const CString &sUNID) { m_sUNID = sUNID; }

	private:
		CString m_sUNID;
		CDockScreenType *m_pType;
		CXMLElement *m_pLocal;
	};

class CEconomyTypeRef
	{
	public:
		CEconomyTypeRef (void) : m_pType(NULL) { }

		inline operator CEconomyType *() const { return m_pType; }
		inline CEconomyType * operator->() const { return m_pType; }

		ALERROR Bind (SDesignLoadCtx &Ctx);
		void LoadUNID (const CString &sUNID) { m_sUNID = sUNID; }
		void Set (DWORD dwUNID);
		inline void Set (CEconomyType *pType) { m_pType = pType; }

	private:
		CString m_sUNID;
		CEconomyType *m_pType;
	};

class CEffectCreatorRef : public CDesignTypeRef<CEffectCreator>
	{
	public:
		CEffectCreatorRef (void) : m_bDelete(false) { }
		~CEffectCreatorRef (void);

		ALERROR Bind (SDesignLoadCtx &Ctx);
		ALERROR CreateBeamEffect (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		ALERROR LoadEffect (SDesignLoadCtx &Ctx, const CString &sUNID, CXMLElement *pDesc, const CString &sAttrib);
		void Set (CEffectCreator *pEffect);

	private:
		bool m_bDelete;
	};

class CItemTableRef : public CDesignTypeRef<CItemTable>
	{
	};

class COverlayTypeRef : public CDesignTypeRef<CEnergyFieldType>
	{
	};

class CShipClassRef : public CDesignTypeRef<CShipClass>
	{
	};

class CShipTableRef : public CDesignTypeRef<CShipTable>
	{
	};

class CSovereignRef : public CDesignTypeRef<CSovereign>
	{
	};

class CStationTypeRef : public CDesignTypeRef<CStationType>
	{
	};

class CSystemMapRef : public CDesignTypeRef<CSystemMap>
	{
	};

//	Classes and structs

class CCurrencyAndValue
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		inline CEconomyType *GetCurrencyType (void) const { return m_pCurrency; }
		inline CurrencyValue GetValue (void) const { return m_iValue; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc);

	private:
		CurrencyValue m_iValue;
		CEconomyTypeRef m_pCurrency;
	};

class CCurrencyAndRange
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		inline CEconomyType *GetCurrencyType (void) const { return m_pCurrency; }
		inline const DiceRange &GetDiceRange (void) const { return m_Value; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc);
		inline CurrencyValue Roll (void) const { return m_Value.Roll(); }

	private:
		DiceRange m_Value;
		CEconomyTypeRef m_pCurrency;
	};

struct SViewportPaintCtx
	{
	SViewportPaintCtx (void)
		{
		pObj = NULL;
		wSpaceColor = 0;
		fNoSelection = false;
		fNoRecon = false;
		fNoDockedShips = false;
		iTick = 0;
		iVariant = 0;
		iDestiny = 0;
		iRotation = 0;
		};

	CSpaceObject *pObj;
	ViewportTransform XForm;			//	Converts from object to screen viewport coordinates
										//		Screen viewport coordinates has positive-Y down.
	WORD wSpaceColor;
	WORD fNoSelection:1;
	WORD fNoRecon:1;
	WORD fNoDockedShips:1;
	WORD wSpare:13;

	//	May be modified by callers
	int iTick;
	int iVariant;
	int iDestiny;
	int iRotation;						//	An angle 0-359
	};

struct SPointInObjectCtx
	{
	SPointInObjectCtx (void) :
			pObjImage(NULL),
			pImage(NULL)
		{ }


	//	Used by CStation
	const CObjectImageArray *pObjImage;

	//	Used by CObjectImageArray
	CG16bitImage *pImage;				//	Image
	RECT rcImage;						//	RECT of valid image
	int xImageOffset;					//	Offset to convert from point coords to image coords
	int yImageOffset;
	};

class CObjectImage : public CDesignType
	{
	public:
		CObjectImage (void);
		CObjectImage (CG16bitImage *pBitmap, bool bFreeBitmap = false);
		~CObjectImage (void);

		CG16bitImage *CreateCopy (CString *retsError = NULL);
		ALERROR Exists (SDesignLoadCtx &Ctx);
		CG16bitImage *GetImage (CString *retsError = NULL);
		CG16bitImage *GetImage (CResourceDb &ResDb, CString *retsError = NULL);
		inline CString GetImageFilename (void) { return m_sBitmap; }
		inline bool HasAlpha (void) { return (m_pBitmap ? m_pBitmap->HasAlpha() : false); }

		inline void ClearMark (void) { m_bMarked = false; }
		ALERROR Lock (SDesignLoadCtx &Ctx);
		inline void Mark (void) { m_bMarked = true; }
		void Sweep (void);

		//	CDesignType overrides
		static CObjectImage *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designImage) ? (CObjectImage *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designImage; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx);
		virtual void OnUnbindDesign (void);

	private:
		CString m_sResourceDb;			//	Resource db
		CString m_sBitmap;				//	Bitmap resource within db
		CString m_sBitmask;				//	Bitmask resource within db
		bool m_bTransColor;				//	If TRUE, m_wTransColor is valid
		WORD m_wTransColor;				//	Transparent color
		bool m_bSprite;					//	If TRUE, convert to sprite after loading
		bool m_bPreMult;				//	If TRUE, image is premultiplied with alpha
		bool m_bLoadOnUse;				//	If TRUE, image is only loaded when needed
		bool m_bFreeBitmap;				//	If TRUE, we free the bitmap when done

		CG16bitImage *m_pBitmap;		//	Loaded image (NULL if not loaded)
		bool m_bMarked;					//	Marked
		bool m_bLocked;					//	Image is never unloaded
	};

class CObjectImageArray : public CObject
	{
	public:
		CObjectImageArray (void);
		CObjectImageArray (const CObjectImageArray &Source);
		~CObjectImageArray (void);
		CObjectImageArray &operator= (const CObjectImageArray &Source);

		ALERROR Init (CG16bitImage *pBitmap, const RECT &rcImage, int iFrameCount, int iTicksPerFrame, bool bFreeBitmap);
		ALERROR Init (DWORD dwBitmapUNID, const RECT &rcImage, int iFrameCount, int iTicksPerFrame);
		ALERROR InitFromXML (CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bResolveNow = false);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);

		void CleanUp (void);
		void CopyImage (CG16bitImage &Dest, int x, int y, int iFrame, int iRotation) const;
		inline DWORD GetBitmapUNID (void) const { return m_dwBitmapUNID; }
		CString GetFilename (void) const;
		inline int GetFrameCount (void) const { return m_iFrameCount; }
		inline CG16bitImage &GetImage (void) const { return *(m_pImage->GetImage()); }
		inline const RECT &GetImageRect (void) const { return m_rcImage; }
		RECT GetImageRect (int iTick, int iRotation, int *retxCenter = NULL, int *retyCenter = NULL) const;
		RECT GetImageRectAtPoint (int x, int y) const;
		inline int GetRotationCount (void) const { return m_iRotationCount; }
		inline int GetTicksPerFrame (void) const { return m_iTicksPerFrame; }
		inline bool HasAlpha (void) const { return (m_pImage ? m_pImage->HasAlpha() : false); }
		bool ImagesIntersect (int iTick, int iRotation, int x, int y, const CObjectImageArray &Image2, int iTick2, int iRotation2) const;
		inline bool IsEmpty (void) const { return m_pImage == NULL; }
		void LoadImage (void);
		void MarkImage (void);
		void PaintImage (CG16bitImage &Dest, int x, int y, int iTick, int iRotation) const;
		void PaintImageGrayed (CG16bitImage &Dest, int x, int y, int iTick, int iRotation) const;
		void PaintImageUL (CG16bitImage &Dest, int x, int y, int iTick, int iRotation) const;
		void PaintImageWithGlow (CG16bitImage &Dest,
								 int x,
								 int y,
								 int iTick,
								 int iRotation,
								 COLORREF rgbGlowColor) const;
		void PaintRotatedImage (CG16bitImage &Dest,
								int x,
								int y,
								int iTick,
								int iRotation) const;
		void PaintScaledImage (CG16bitImage &Dest,
							   int x,
							   int y,
							   int iTick,
							   int iRotation,
							   int cxWidth,
							   int cyHeight) const;
		void PaintSilhoutte (CG16bitImage &Dest,
							 int x,
							 int y,
							 int iTick,
							 int iRotation,
							 WORD wColor) const;
		bool PointInImage (int x, int y, int iTick, int iRotation) const;
		bool PointInImage (SPointInObjectCtx &Ctx, int x, int y) const;
		void PointInImageInit (SPointInObjectCtx &Ctx, int iTick, int iRotation) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void SetRotationCount (int iRotationCount);
		void TakeHandoff (CObjectImageArray &Source);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		enum BlendingTypes
			{
			blendNormal,
			blendLighten,
			};

		struct OffsetStruct
			{
			int x;
			int y;
			};

		void ComputeRotationOffsets (void);
		void ComputeRotationOffsets (int xOffset, int yOffset);
		int ComputeSourceX (int iTick) const;
		void CopyFrom (const CObjectImageArray &Source);
		void GenerateGlowImage (int iRotation) const;
		void GenerateScaledImages (int iRotation, int cxWidth, int cyHeight) const;

		DWORD m_dwBitmapUNID;				//	UNID of bitmap (0 if none)
		CObjectImage *m_pImage;				//	Image (if UNID is 0, we own this structure)
		RECT m_rcImage;
		int m_iFrameCount;
		int m_iTicksPerFrame;
		int m_iFlashTicks;
		int m_iRotationCount;
		int m_iRotationOffset;
		OffsetStruct *m_pRotationOffset;
		int m_iBlending;

		//	Glow effect
		mutable CG16bitImage *m_pGlowImages;
		mutable CG16bitImage *m_pScaledImages;

	friend CObjectClass<CObjectImageArray>;
	};

const DWORD DEFAULT_SELECTOR_ID = 0;

class CCompositeImageSelector
	{
	public:
		CCompositeImageSelector (void) { }

		bool operator== (const CCompositeImageSelector &Val) const;

		void AddShipwreck (DWORD dwID, CShipClass *pWreckClass, int iVariant = -1);
		void AddVariant (DWORD dwID, int iVariant);
		inline void DeleteAll (void) { m_Sel.DeleteAll(); }
		inline int GetCount (void) const { return m_Sel.GetCount(); }
		CShipClass *GetShipwreckClass (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		CObjectImageArray &GetShipwreckImage (DWORD dwID = DEFAULT_SELECTOR_ID) const;
		int GetVariant (DWORD dwID) const;
		inline bool HasShipwreckImage (DWORD dwID = DEFAULT_SELECTOR_ID) const { return (GetShipwreckClass(dwID) != NULL); }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		struct SEntry
			{
			DWORD dwID;
			int iVariant;
			DWORD dwExtra;
			};

		SEntry *FindEntry (DWORD dwID) const;

		TArray<SEntry> m_Sel;
	};

class IImageEntry
	{
	public:
		virtual ~IImageEntry (void) { }

		inline DWORD GetID (void) { return m_dwID; }
		virtual void GetImage (const CCompositeImageSelector &Selector, CObjectImageArray *retImage) = 0;
		virtual int GetMaxLifetime (void) const { return 0; }
		virtual int GetVariantCount (void) = 0;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CIDCounter &IDGen, CXMLElement *pDesc) { return NOERROR; }
		virtual void InitSelector (CCompositeImageSelector *retSelector) { }
		virtual void InitSelector (int iVariant, CCompositeImageSelector *retSelector) { }
		virtual bool IsConstant (void) = 0;
		virtual void LoadImage (void) { }
		virtual void MarkImage (void) { }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

	protected:
		DWORD m_dwID;
	};

class CCompositeImageDesc
	{
	public:
		enum Modifiers
			{
			modStationDamage =		0x00000001,
			};

		CCompositeImageDesc (void);
		~CCompositeImageDesc (void);

		CObjectImageArray &GetImage (const CCompositeImageSelector &Selector, DWORD dwModifiers = 0, int *retiRotation = NULL) const;
		int GetMaxLifetime (void) const;
		inline IImageEntry *GetRoot (void) const { return m_pRoot; }
		inline int GetVariantCount (void) { return (m_pRoot ? m_pRoot->GetVariantCount() : 0); }
		static ALERROR InitEntryFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CIDCounter &IDGen, IImageEntry **retpEntry);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitSelector (CCompositeImageSelector *retSelector);
		void InitSelector (int iVariant, CCompositeImageSelector *retSelector) { if (m_pRoot) m_pRoot->InitSelector(iVariant, retSelector); }
		inline bool IsConstant (void) const { return m_bConstant; }
		inline bool IsEmpty (void) { return (GetVariantCount() == 0); }
		void LoadImage (void);
		void LoadImage (const CCompositeImageSelector &Selector);
		void MarkImage (void);
		void MarkImage (const CCompositeImageSelector &Selector);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		static void Reinit (void);

	private:
		struct SCacheEntry
			{
			CCompositeImageSelector Selector;
			DWORD dwModifiers;
			CObjectImageArray Image;
			};

		void ApplyModifiers (DWORD dwModifiers, CObjectImageArray *retImage) const;
		SCacheEntry *FindCacheEntry (const CCompositeImageSelector &Selector, DWORD dwModifiers) const;
		static void InitDamagePainters (void);
		static void PaintDamage (CG16bitImage &Dest, const RECT &rcDest, int iCount, IEffectPainter *pPainter);

		IImageEntry *m_pRoot;
		bool m_bConstant;
		mutable TArray<SCacheEntry> m_Cache;
	};

//	Damage

enum BeamTypes
	{
	beamUnknown =				-1,

	beamHeavyBlaster =			0,
	beamLaser =					1,
	beamLightning =				2,
	beamStarBlaster =			3,

	beamGreenParticle =			4,
	beamBlueParticle =			5,
	beamElectron =				6,
	beamPositron =				7,
	beamGraviton =				8,
	beamBlaster =				9,
	beamGreenLightning =		10,
	beamParticle =				11,
	};

enum DamageTypes
	{
	damageError			= -100,					//	invalid damage
	damageGeneric		= -1,					//	generic damage

	damageLaser			= 0,					//	standard lasers
	damageKinetic		= 1,					//	mass drivers
	damageParticle		= 2,					//	charged particle beam
	damageBlast			= 3,					//	chemical explosives
	damageIonRadiation	= 4,					//	ionizing radiation
	damageThermonuclear	= 5,					//	hydrogen bomb
	damagePositron		= 6,					//	anti-matter charged particles
	damagePlasma		= 7,					//	fusion weapons
	damageAntiMatter	= 8,					//	anti-matter torpedo
	damageNano			= 9,					//	nano-machines
	damageGravitonBeam	= 10,					//	graviton beam
	damageSingularity	= 11,					//	spacetime weapons
	damageDarkAcid		= 12,					//	exotics
	damageDarkSteel		= 13,					//	exotics
	damageDarkLightning	= 14,					//	exotics
	damageDarkFire		= 15,					//	exotics

	damageCount			= 16
	};

enum SpecialDamageTypes
	{
	specialRadiation	= 0,
	specialBlinding		= 1,
	specialEMP			= 2,
	specialDeviceDamage	= 3,
	specialDisintegration = 4,
	specialMomentum		= 5,
	specialShieldDisrupt = 6,
	specialWMD			= 7,
	specialMining		= 8,

	specialDeviceDisrupt= 9,
	specialWormhole		= 10,
	specialFuel			= 11,
	specialShatter		= 12,
	//	max 15 (because it is used in CItemEnhancement)
	};

class DamageDesc
	{
	public:
		enum Flags 
			{
			flagAverageDamage = 0x00000001,
			};

		DamageDesc (void) { }
		DamageDesc (DamageTypes iType, const DiceRange &Damage) : m_iType(iType),
				m_Damage(Damage),
				m_iBonus(0),
				m_iCause(killedByDamage),
				m_EMPDamage(0),
				m_MomentumDamage(0),
				m_RadiationDamage(0),
				m_DisintegrationDamage(0),
				m_DeviceDisruptDamage(0),
				m_BlindingDamage(0),
				m_SensorDamage(0),
				m_ShieldDamage(0),
				m_WormholeDamage(0),
				m_FuelDamage(0),
				m_fNoSRSFlash(0),
				m_fAutomatedWeapon(0),
				m_DeviceDamage(0),
				m_MassDestructionAdj(0),
				m_MiningAdj(0),
				m_ShatterDamage(0),
				m_dwSpare2(0)
			{ }

		inline void AddBonus (int iBonus) { m_iBonus += iBonus; }
		bool CausesSRSFlash (void) const { return (m_fNoSRSFlash ? false : true); }
		Metric GetAverageDamage (void);
		inline DestructionTypes GetCause (void) const { return m_iCause; }
		inline DamageTypes GetDamageType (void) const { return m_iType; }
		CString GetDesc (DWORD dwFlags = 0);
		int GetMinDamage (void);
		int GetMaxDamage (void);
		bool IsAutomatedWeapon (void) const { return (m_fAutomatedWeapon ? true : false); }
		bool IsEnergyDamage (void) const;
		bool IsMatterDamage (void) const;
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CString &sAttrib);
		void ReadFromStream (SLoadCtx &Ctx);
		int RollDamage (void) const;
		inline void SetAutomatedWeapon (void) { m_fAutomatedWeapon = true; }
		inline void SetCause (DestructionTypes iCause) { m_iCause = iCause; }
		void SetDamage (int iDamage);
		inline void SetDamageType (DamageTypes iType) { m_iType = iType; }
		inline void SetNoSRSFlash (void) { m_fNoSRSFlash = true; }
		void WriteToStream (IWriteStream *pStream) const;

		inline int GetBlindingDamage (void) const { return (int)m_BlindingDamage; }
		inline int GetDeviceDamage (void) const { return (int)m_DeviceDamage; }
		inline int GetDeviceDisruptDamage (void) const { return (int)m_DeviceDisruptDamage; }
		inline int GetDisintegrationDamage (void) const { return (int)m_DisintegrationDamage; }
		inline int GetEMPDamage (void) const { return (int)m_EMPDamage; }
		inline int GetMomentumDamage (void) const { return (int)m_MomentumDamage; }
		inline int GetRadiationDamage (void) const { return (int)m_RadiationDamage; }
		inline int GetShatterDamage (void) const { return (int)m_ShatterDamage; }
		inline int GetShieldDamage (void) const { return (int)m_ShieldDamage; }

		inline int GetMassDestructionAdj (void) const { return (int)(m_MassDestructionAdj ? (2 * (m_MassDestructionAdj * m_MassDestructionAdj) + 2) : 0); }
		inline int GetMiningAdj (void) const { return (int)(m_MiningAdj ? (2 * (m_MiningAdj * m_MiningAdj) + 2) : 0); }
		inline int GetShieldDamageLevel (void) const { return (int)m_ShieldDamage; }

	private:
		ALERROR LoadTermFromXML (SDesignLoadCtx &Ctx, const CString &sType, const CString &sArg);
		ALERROR ParseTerm (SDesignLoadCtx &Ctx, char *pPos, CString *retsKeyword, CString *retsValue, char **retpPos);

		DamageTypes m_iType;					//	Type of damage
		DiceRange m_Damage;						//	Amount of damage
		int m_iBonus;							//	Bonus to damage (%)
		DestructionTypes m_iCause;				//	Cause of damage

		//	Extra damage
		DWORD m_EMPDamage:3;					//	Ion (paralysis) damage
		DWORD m_MomentumDamage:3;				//	Momentum damage
		DWORD m_RadiationDamage:3;				//	Radiation damage
		DWORD m_DeviceDisruptDamage:3;			//	Disrupt devices damage
		DWORD m_BlindingDamage:3;				//	Optical sensor damage
		DWORD m_SensorDamage:3;					//	Long-range sensor damage
		DWORD m_WormholeDamage:3;				//	Teleport
		DWORD m_FuelDamage:3;					//	Drain fuel
		DWORD m_DisintegrationDamage:3;			//	Disintegration damage
		DWORD m_dwSpare1:3;

		DWORD m_fNoSRSFlash:1;					//	If TRUE, damage should not cause SRS flash
		DWORD m_fAutomatedWeapon:1;				//	TRUE if this damage is caused by automated weapon

		DWORD m_DeviceDamage:3;					//	Damage to devices
		DWORD m_MassDestructionAdj:3;			//	Adj for mass destruction
		DWORD m_MiningAdj:3;					//	Adj for mining capability
		DWORD m_ShatterDamage:3;				//	Shatter damage
		DWORD m_dwSpare2:20;

		BYTE m_ShieldDamage;					//	Shield damage
		BYTE m_Spare1;
		BYTE m_Spare2;
		BYTE m_Spare3;
	};

enum EDamageResults
	{
	damageNoDamage =				0,
	damageAbsorbedByShields =		1,
	damageArmorHit =				2,
	damageStructuralHit =			3,
	damageDestroyed =				4,
	damagePassthrough =				5,	//	When we hit another missile (or small obj) we pass through
	damagePassthroughDestroyed =	6,	//	Target destroyed, but we pass through
	damageDestroyedAbandoned =		7,	//	Station was abandoned, but object not destroyed
	};

struct SDamageCtx
	{
	CSpaceObject *pObj;							//	Object hit
	CWeaponFireDesc *pDesc;						//	WeaponFireDesc
	DamageDesc Damage;							//	Damage
	int iDirection;								//	Direction that hit came from
	CVector vHitPos;							//	Hit at this position
	CSpaceObject *pCause;						//	Object that directly caused the damage
	CDamageSource Attacker;						//	Ultimate attacker

	int iDamage;								//	Damage hp
	int iSectHit;								//	Armor section hit on object

	//	These are used within armor/shield processing
	int iHPLeft;								//	HP left on armor/shields (before damage)
	int iAbsorb;								//	Damage absorbed by shields
	int iShieldDamage;							//	Damage taken by shields
	int iOriginalAbsorb;						//	Computed absorb value, if shot had not been reflected
	int iOriginalShieldDamage;					//	Computed shield damage value, if shot had not been reflected

	//	Damage effects
	bool bBlind;								//	If true, shot will blind the target
	int iBlindTime;
	bool bDeviceDisrupt;						//	If true, shot will disrupt devices
	int iDisruptTime;
	bool bDeviceDamage;							//	If true, shot will damage devices
	bool bDisintegrate;							//	If true, shot will disintegrate target
	bool bParalyze;								//	If true, shot will paralyze the target
	int iParalyzeTime;
	bool bRadioactive;							//	If true, shot will irradiate the target
	bool bReflect;								//	If true, armor/shields reflected the shot
	bool bShatter;								//	If true, shot will shatter the target
	};

struct SDestroyCtx
	{
	SDestroyCtx (void) :
		pObj(NULL),
		pWreck(NULL),
		iCause(removedFromSystem),
		bResurrectPending(false),
		pResurrectedObj(NULL) { }

	CSpaceObject *pObj;							//	Object destroyed
	CDamageSource Attacker;						//	Ultimate attacker
	CSpaceObject *pWreck;						//	Wreck left behind
	DestructionTypes iCause;					//	Cause of damage

	bool bResurrectPending;						//	TRUE if this object will be resurrected
	CSpaceObject *pResurrectedObj;				//	Pointer to resurrected object
	};

class DamageTypeSet
	{
	public:
		DamageTypeSet (void) : m_dwSet(0) { }

		ALERROR InitFromXML (const CString &sAttrib);
		void Add (int iType) { if (iType > damageGeneric) m_dwSet |= (1 << iType); }
		bool InSet (int iType) { return (iType <= damageGeneric ? false : ((m_dwSet & (1 << iType)) ? true : false)); }
		void Remove (int iType) { if (iType > damageGeneric) m_dwSet &= ~(1 << iType); }

	private:
		DWORD m_dwSet;
	};

//	WeaponFireDesc

enum FireTypes
	{
	ftArea,
	ftBeam,
	ftMissile,
	ftParticles,
	ftRadius,
	};

enum ParticleTypes
	{
	particleImage,
	particlePlasma,
	particleBolt,
	};

//	Communications

//	DO NOT CHANGE NUMBERS
//	NOTE: These numbers must match-up with CSovereign.cpp and with the
//	definitions in the <Sovereign> elements. And with Utilities.cpp

enum MessageTypes
	{
	msgNone =						0,	//	None
	msgAttack =						1,	//	Order to attack (pParam1 is target)
	msgDestroyBroadcast =			2,	//	Broadcast order to destroy target (pParam1 is target)
	msgHitByFriendlyFire =			3,	//	Hit by friendly fire (pParam1 is attacker)
	msgQueryEscortStatus =			4,	//	Replies with Ack if is escorting pParam1
	msgQueryFleetStatus =			5,	//	Replies with Ack if is in formation with pParam1
	msgEscortAttacked =				6,	//	pParam1 attacked escort ship (or defender)
	msgEscortReportingIn =			7,	//	pParam1 is now escorting the ship
	msgWatchTargets =				8,	//	"watch your targets!"
	msgNiceShooting =				9,	//	"nice shooting!"
	msgFormUp =						10,	//	Enter formation (dwParam2 is formation place)
	msgBreakAndAttack =				11,	//	Break formation and attack
	msgQueryCommunications =		12,	//	Returns comms capabilities
	msgAbort =						13,	//	Cancel attack order
	msgWait =						14,	//	Wait there for a while
	msgQueryWaitStatus =			15,	//	Replies with Ack if waiting
	msgAttackInFormation =			16,	//	Fire weapon while staying in formation
	msgAttackDeter =				17, //	Attack target, but return after a while (pParam1 is target)
	msgQueryAttackStatus =			18, //	Replies with Ack if attacking a target
	};

enum ResponseTypes
	{
	resNoAnswer =					0,
	resAck =						1,

	resCanBeInFormation =			0x0010,
	resCanAttack =					0x0020,
	resCanBreakAndAttack =			0x0040,
	resCanAbortAttack =				0x0080,
	resCanWait =					0x0100,
	resCanFormUp =					0x0200,
	resCanAttackInFormation =		0x0400,
	};

class CCommunicationsHandler
	{
	public:
		struct SMessage
			{
			CString sMessage;
			CString sShortcut;

			SEventHandlerDesc InvokeEvent;
			SEventHandlerDesc OnShowEvent;
			};

		CCommunicationsHandler (void);
		~CCommunicationsHandler (void);

		ALERROR InitFromXML (CXMLElement *pDesc, CString *retsError);

		int FindMessage (const CString &sMessage) const;
		inline int GetCount (void) const { return m_Messages.GetCount(); }
		inline const SMessage &GetMessage (int iIndex) { return m_Messages[iIndex]; }
		void Merge (CCommunicationsHandler &New);

	private:
		int FindByShortcut (const CString &sShortcut, int *retiInsert = NULL);
		TArray<SMessage> m_Messages;
	};

//	Equipment (Abilities)

enum Abilities
	{
	ablUnknown =				-1,

	ablShortRangeScanner =		0,		//	Main viewscreen
	ablLongRangeScanner =		1,		//	LRS
	ablSystemMap =				2,		//	System map display
	ablAutopilot =				3,		//	Autopilot
	ablExtendedScanner =		4,		//	Extended marks on viewscreen
	ablTargetingSystem =		5,		//	Targeting computer
	ablGalacticMap =			6,		//	Galactic map display
	};

enum AbilityModifications
	{
	ablModificationUnknown =	-1,

	ablInstall =				0,		//	Install the ability
	ablRemove =					1,		//	Remove the ability (if installed)
	ablDamage =					2,		//	Damage the ability (if installed)
	ablRepair =					3,		//	Repair the ability (if damaged)
	};

enum AbilityModificationOptions
	{
	ablOptionUnknown =			0x00000000,

	ablOptionNoMessage =		0x00000001,	//	Do not show a message to player
	};

enum AbilityStatus
	{
	ablStatusUnknown =			-1,

	ablUninstalled =			0,		//	>0 means that is installed (though it could be damaged)
	ablInstalled =				1,
	ablDamaged =				2,
	};

//	Item Types

struct CItemCriteria
	{
	CItemCriteria (void);
	CItemCriteria (const CItemCriteria &Copy);
	~CItemCriteria (void);
	CItemCriteria &operator= (const CItemCriteria &Copy);

	DWORD dwItemCategories;			//	Set of ItemCategories to match on
	DWORD dwExcludeCategories;		//	Categories to exclude
	DWORD dwMustHaveCategories;		//	ANDed categories

	WORD wFlagsMustBeSet;			//	These flags must be set
	WORD wFlagsMustBeCleared;		//	These flags must be cleared

	bool bUsableItemsOnly;			//	Item must be usable
	bool bExcludeVirtual;			//	Exclude virtual items
	bool bInstalledOnly;			//	Item must be installed
	bool bNotInstalledOnly;			//	Item must not be installed

	TArray<CString> ModifiersRequired;		//	Required modifiers
	TArray<CString> ModifiersNotAllowed;	//	Exclude these modifiers
	TArray<CString> SpecialAttribRequired;	//	Special required attributes
	TArray<CString> SpecialAttribNotAllowed;//	Exclude these special attributes
	CString Frequency;				//	If not blank, only items with these frequencies

	int iEqualToLevel;				//	If not -1, only items of this level
	int iGreaterThanLevel;			//	If not -1, only items greater than this level
	int iLessThanLevel;				//	If not -1, only items less than this level
	int iEqualToPrice;				//	If not -1, only items at this price
	int iGreaterThanPrice;			//	If not -1, only items greater than this price
	int iLessThanPrice;				//	If not -1, only items less than this price
	int iEqualToMass;				//	If not -1, only items of this mass (in kg)
	int iGreaterThanMass;			//	If not -1, only items greater than this mass (in kg)
	int iLessThanMass;				//	If not -1, only items less than this mass (in kg)

	ICCItem *pFilter;				//	Filter returns Nil for excluded items
	};

enum ItemEnhancementTypes
	{
	etNone =							0x0000,
	etBinaryEnhancement =				0x0001,
	etLoseEnhancement =					0x0002,	//	Lose enhancements

	etStrengthen =						0x0100,	//	+hp, data1 = %increase (10% increments)
	etRegenerate =						0x0200,	//	data1 = rate
	etReflect =							0x0300,	//	data2 = damage type reflected
	etRepairOnHit =						0x0400,	//	repair damage on hit, data2 = damage type of hit
	etResist =							0x0500,	//	-damage, data1 = %damage adj
	etResistEnergy =					0x0600,	//	-energy damage, data1 = %damage adj (90%, 80%, etc)
	etResistMatter =					0x0700,	//	-matter damage, data1 = %damage adj (90%, 80%, etc)
	etResistByLevel =					0x0800,	//	-damage, data1 = %damage adj, data2 = damage type
	etResistByDamage =					0x0900,	//	-damage, data1 = %damage adj, data2 = damage type
	etResistByDamage2 =					0x0a00,	//	-damage, data1 = %damage adj, data2 = damage type
	etSpecialDamage =					0x0b00,	//	Immunity to damage effects:
												//		data2 = 0: immune to radiation
												//		data2 = 1: immune to blinding
												//		... see SpecialDamageTypes
	etImmunityIonEffects =				0x0c00,	//	Immunity to ion effects (blinding, EMP, etc.)
												//		(if disadvantage, interferes with shields)
	etPhotoRegenerate =					0x0d00,	//	regen near sun
	etPhotoRecharge =					0x0e00,	//	refuel near sun
	etPowerEfficiency =					0x0f00,	//	power usage decrease, 01 = 90%/110%, 02 = 80%/120%
	etSpeed =							0x1000,	//	decrease cycle time
	etTurret =							0x1100,	//	weapon turret, data1 is angle
	etMultiShot =						0x1200,	//	multiple shots, data2 = count, data1 = %weakening

	etData1Mask =						0x000f,	//	4-bits of data (generally for damage adj)
	etData2Mask =						0x00f0,	//	4-bits of data (generally for damage type)
	etTypeMask =						0x7f00,	//	Type
	etDisadvantage =					0x8000,	//	If set, this is a disadvantage
	};

enum EnhanceItemStatus
	{
	eisOK,										//	Enhancement OK
	eisNoEffect,								//	Nothing happens
	eisAlreadyEnhanced,							//	Already has this exact enhancement
	eisWorse,									//	A disadvantage was made worse
	eisRepaired,								//	Disadvantage was repaired
	eisEnhancementRemoved,						//	Enhancement removed
	eisEnhancementReplaced,						//	Enhancement replaced by another enhancement
	eisBetter,									//	Enhancement made better
	};

class CItemEnhancement
	{
	public:
		CItemEnhancement (void) : m_dwID(OBJID_NULL), m_dwMods(0), m_pEnhancer(NULL), m_iExpireTime(-1) { }
		CItemEnhancement (DWORD dwMods) : m_dwID(OBJID_NULL), m_dwMods(dwMods), m_pEnhancer(NULL), m_iExpireTime(-1) { }

		inline DWORD AsDWORD (void) const { return m_dwMods; }
		EnhanceItemStatus Combine (CItemEnhancement Enhancement);
		int GetAbsorbAdj (const DamageDesc &Damage) const;
		int GetActivateRateAdj (void) const;
		int GetDamageAdj (const DamageDesc &Damage) const;
		int GetDamageBonus (void) const;
		inline DamageTypes GetDamageType (void) const { return (DamageTypes)(DWORD)((m_dwMods & etData2Mask) >> 4); }
		CString GetEnhancedDesc (const CItem &Item, CSpaceObject *pInstalled, CInstalledDevice *pDevice = NULL) const;
		int GetEnhancedRate (int iRate) const;
		inline CItemType *GetEnhancementType (void) const { return m_pEnhancer; }
		inline int GetExpireTime (void) const { return m_iExpireTime; }
		int GetHPAdj (void) const;
		inline DWORD GetID (void) const { return m_dwID; }
		inline int GetLevel (void) const { return (int)(DWORD)(m_dwMods & etData1Mask); }
		inline int GetLevel2 (void) const { return (int)(DWORD)((m_dwMods & etData2Mask) >> 4); }
		inline DWORD GetModCode (void) const { return m_dwMods; }
		int GetPowerAdj (void) const;
		inline ItemEnhancementTypes GetType (void) const { return (ItemEnhancementTypes)(m_dwMods & etTypeMask); }
		int GetValueAdj (const CItem &Item) const;
		inline bool IsBlindingImmune (void) const { return IsIonEffectImmune() || ((GetType() == etSpecialDamage) && GetLevel2() == specialBlinding && !IsDisadvantage()); }
		inline bool IsDecaying (void) const { return ((GetType() == etRegenerate) && IsDisadvantage()); }
		inline bool IsDeviceDamageImmune (void) const { return IsIonEffectImmune() || ((GetType() == etSpecialDamage) && GetLevel2() == specialDeviceDamage && !IsDisadvantage()); }
		inline bool IsDisadvantage (void) const { return ((m_dwMods & etDisadvantage) ? true : false); }
		inline bool IsDisintegrationImmune (void) const { return ((GetType() == etSpecialDamage) && GetLevel2() == specialDisintegration && !IsDisadvantage()); }
		inline bool IsEMPImmune (void) const { return IsIonEffectImmune() || ((GetType() == etSpecialDamage) && GetLevel2() == specialEMP && !IsDisadvantage()); }
		inline bool IsEmpty (void) const { return (m_dwMods == 0 && m_pEnhancer == NULL); }
		inline bool IsEnhancement (void) const { return (m_dwMods && !IsDisadvantage()); }
		bool IsEqual (const CItemEnhancement &Comp) const;
		inline bool IsNotEmpty (void) const { return !IsEmpty(); }
		inline bool IsPhotoRecharge (void) const { return ((GetType() == etPhotoRecharge) && !IsDisadvantage()); }
		inline bool IsPhotoRegenerating (void) const { return ((GetType() == etPhotoRegenerate) && !IsDisadvantage()); }
		inline bool IsRadiationImmune (void) const { return ((GetType() == etSpecialDamage) && GetLevel2() == 0 && !IsDisadvantage()); }
		inline bool IsRegenerating (void) const { return ((GetType() == etRegenerate) && !IsDisadvantage()); }
		inline bool IsReflective (void) const { return ((GetType() == etReflect) && !IsDisadvantage()); }
		bool IsReflective (const DamageDesc &Damage, int *retiReflectChance = NULL) const;
		inline bool IsShieldInterfering (void) const { return ((GetType() == etImmunityIonEffects) && IsDisadvantage()); }
		void ReadFromStream (DWORD dwVersion, IReadStream *pStream);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetEnhancementType (CItemType *pType) { m_pEnhancer = pType; }
		inline void SetExpireTime (int iTime) { m_iExpireTime = iTime; }
		inline void SetID (DWORD dwID) { m_dwID = dwID; }
		inline void SetModCode (DWORD dwMods) { m_dwMods = dwMods; }
		void WriteToStream (IWriteStream *pStream) const;

	private:
		inline bool IsIonEffectImmune (void) const { return ((GetType() == etImmunityIonEffects) && !IsDisadvantage()); }
		static int Level2Bonus (int iLevel, bool bDisadvantage = false);
		static int Level2DamageAdj (int iLevel, bool bDisadvantage = false);

		DWORD m_dwID;							//	Global ID
		DWORD m_dwMods;							//	Mod code
		CItemType *m_pEnhancer;					//	Item that added this mod (may be NULL)
		int m_iExpireTime;						//	Universe tick when mod expires (-1 == no expiration)
	};

const int MAX_ITEM_CHARGES =			0x7fff;

class CItem
	{
	public:
		CItem (void);
		CItem (const CItem &Copy);
		CItem (CItemType *pItemType, int iCount);
		~CItem (void);
		CItem &operator= (const CItem &Copy);

		DWORD AddEnhancement (const CItemEnhancement &Enhancement);
		inline void ClearDamaged (void) { m_dwFlags &= ~flagDamaged; }
		inline void ClearDisrupted (void) { if (m_pExtra) m_pExtra->m_dwDisruptedTime = 0; }
		inline void ClearEnhanced (void) { m_dwFlags &= ~flagEnhanced; }
		inline void ClearModFlag (int iMod) { m_dwFlags &= ~Mod2Flags(iMod); }
		static CItem CreateItemByName (const CString &sName, const CItemCriteria &Criteria, bool bActualName = false);
		bool IsEqual (const CItem &Item) const;
		bool FireCanBeInstalled (CSpaceObject *pSource, CString *retsError) const;
		bool FireCanBeUninstalled (CSpaceObject *pSource, CString *retsError) const;
		void FireOnAddedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced, EnhanceItemStatus iStatus) const;
		void FireOnDisabled (CSpaceObject *pSource) const;
		void FireOnEnabled (CSpaceObject *pSource) const;
		void FireOnInstall (CSpaceObject *pSource) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		bool FireOnReactorOverload (CSpaceObject *pSource) const;
		void FireOnRemovedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced) const;
		void FireOnUninstall (CSpaceObject *pSource) const;
		inline int GetCharges (void) const { return (m_pExtra ? (int)m_pExtra->m_dwCharges : 0); }
		inline int GetCount (void) const { return (int)m_dwCount; }
		inline CEconomyType *GetCurrencyType (void) const;
		inline CString GetData (const CString &sAttrib) const { return (m_pExtra ? m_pExtra->m_Data.GetData(sAttrib) : NULL_STR); }
		inline const CString &GetDesc (void) const;
		DWORD GetDisruptedDuration (void) const;
		CString GetEnhancedDesc (CSpaceObject *pInstalled = NULL) const;
		inline int GetInstalled (void) const { return (int)(char)m_dwInstalled; }
		inline const CItemEnhancement &GetMods (void) const { return (m_pExtra ? m_pExtra->m_Mods : m_NullMod); }
		static const CItem &GetNullItem (void) { return m_NullItem; }
		static const CItemEnhancement &GetNullMod (void) { return m_NullMod; }
		CString GetReference (CItemCtx &Ctx, int iVariant = -1, DWORD dwFlags = 0) const;
		bool GetReferenceDamageAdj (CSpaceObject *pInstalled, DWORD dwFlags, int *retiHP, int *retArray) const;
		bool GetReferenceDamageType (CSpaceObject *pInstalled, int iVariant, DWORD dwFlags, DamageTypes *retiDamage, CString *retsReference) const;
		CString GetNounPhrase (DWORD dwFlags) const;
		int GetTradePrice (CSpaceObject *pObj, bool bActual = false) const;
		inline CItemType *GetType (void) const { return m_pItemType; }
		int GetValue (bool bActual = false) const;
		inline bool HasMods (void) const { return (m_pExtra && m_pExtra->m_Mods.IsNotEmpty()); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		inline bool IsDamaged (void) const { return (m_dwFlags & flagDamaged ? true : false); }
		inline bool IsDisrupted (void) const;
		inline bool IsDisrupted (DWORD dwNow) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= dwNow) : false); }
		inline bool IsEnhanced (void) const { return (m_dwFlags & flagEnhanced ? true : false); }
		inline bool IsInstalled (void) const { return (m_dwInstalled != 0xff); }
		inline bool IsMarkedForDelete (void) { return (m_dwCount == 0xffff); }
		inline void MarkForDelete (void) { m_dwCount = 0xffff; }
		bool RemoveEnhancement (DWORD dwID);
		inline void SetCharges (int iCharges) { Extra(); m_pExtra->m_dwCharges = iCharges; }
		inline void SetCount (int iCount) { m_dwCount = (DWORD)iCount; }
		inline void SetDamaged (void) { m_dwFlags |= flagDamaged; }
		inline void SetDamaged (bool bDamaged) { ClearDamaged(); if (bDamaged) SetDamaged(); }
		inline void SetData (const CString &sAttrib, const CString &sData) { Extra(); m_pExtra->m_Data.SetData(sAttrib, sData); }
		void SetDisrupted (DWORD dwDuration);
		inline void SetEnhanced (void) { m_dwFlags |= flagEnhanced; }
		inline void SetEnhanced (bool bEnhanced) { ClearEnhanced(); if (bEnhanced) SetEnhanced(); }
		inline void SetInstalled (int iInstalled) { m_dwInstalled = (BYTE)(char)iInstalled; }

		static CString GenerateCriteria (const CItemCriteria &Criteria);
		static void InitCriteriaAll (CItemCriteria *retCriteria);
		static void ParseCriteria (const CString &sCriteria, CItemCriteria *retCriteria);
		bool MatchesCriteria (const CItemCriteria &Criteria) const;

		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		void ReadFromCCItem (CCodeChain &CC, ICCItem *pBuffer);
		ICCItem *WriteToCCItem (CCodeChain &CC) const;

	private:
		enum PFlags
			{
			flagDamaged =		0x01,			//	Item is damaged

			flagEnhanced =		0x10,			//	Item is enhanced (Mod 1)
			flagMod2 =			0x20,			//	Mod 2
			flagMod3 =			0x40,			//	Mod 3
			flagMod4 =			0x80,			//	Mod 4
			};

		struct SExtra
			{
			DWORD m_dwCharges;					//	Charges for items
			DWORD m_dwVariant;					//	Affects stats based on type (e.g., for armor, this is maxHP)
			DWORD m_dwDisruptedTime;			//	If >0, the tick on which disruption expires

			CItemEnhancement m_Mods;			//	Class-specific modifications (e.g., armor enhancements)

			CAttributeDataBlock m_Data;			//	Opaque data
			};

		void Extra (void);
		static bool IsExtraEmpty (const SExtra *pExtra);
		bool IsExtraEqual (SExtra *pSrc) const;
		DWORD Mod2Flags (int iMod) const;

		CItemType *m_pItemType;

		DWORD m_dwCount:16;						//	Number of items
		DWORD m_dwFlags:8;						//	Miscellaneous flags
		DWORD m_dwInstalled:8;					//	Location where item is installed

		SExtra *m_pExtra;						//	Extra data (may be NULL)

		static CItem m_NullItem;
		static CItemEnhancement m_NullMod;
	};

class CItemList
	{
	public:
		CItemList (void);
		~CItemList (void);
		CItemList &operator= (const CItemList &Copy);

		void AddItem (const CItem &Item);
		void DeleteAll (void);
		void DeleteItem (int iIndex);
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CItem &GetItem (int iIndex) { return *m_List[iIndex]; }
		inline const CItem &GetItem (int iIndex) const { return *m_List[iIndex]; }
		void ReadFromStream (SLoadCtx &Ctx);
		void SortItems (void);
		void WriteToStream (IWriteStream *pStream);

	private:
		TArray<CItem *> m_List;
	};

class CItemListManipulator
	{
	public:
		CItemListManipulator (CItemList &ItemList);
		~CItemListManipulator (void);

		void AddItem (const CItem &Item);
		void AddItems (const CItemList &ItemList);

		inline int GetCount (void) { return m_ViewMap.GetCount(); }
		inline int GetCursor (void) { return m_iCursor; }
		inline void SetCursor (int iCursor) { m_iCursor = iCursor; }
		bool SetCursorAtItem (const CItem &Item);
		void SetFilter (const CItemCriteria &Filter);
		bool Refresh (const CItem &Item);

		inline bool IsCursorValid (void) { return m_iCursor != -1; }
		bool MoveCursorBack (void);
		bool MoveCursorForward (void);
		void ResetCursor (void);

		DWORD AddItemEnhancementAtCursor (const CItemEnhancement &Mods, int iCount = 1);
		void ClearDisruptedAtCursor (int iCount = 1);
		void DeleteAtCursor (int iCount);
		void DeleteMarkedItems (void);
		const CItem &GetItemAtCursor (void);
		CItem *GetItemPointerAtCursor (void);
		void MarkDeleteAtCursor (int iCount);
		void RemoveItemEnhancementAtCursor (DWORD dwID, int iCount = 1);
		void SetChargesAtCursor (int iCharges, int iCount = 1);
		void SetCountAtCursor (int iCount);
		void SetDamagedAtCursor (bool bDamaged, int iCount = 1);
		void SetDataAtCursor (const CString &sAttrib, const CString &sData, int iCount = 1);
		void SetDisruptedAtCursor (DWORD dwDuration, int iCount = 1);
		void SetEnhancedAtCursor (bool bEnhanced);
		void SetInstalledAtCursor (int iInstalled);
		void TransferAtCursor (int iCount, CItemList &DestList);

	private:
		int FindItem (const CItem &Item);
		void GenerateViewMap (void);
		void MoveItemTo (const CItem &NewItem, const CItem &OldItem);

		CItemList &m_ItemList;
		int m_iCursor;
		CIntArray m_ViewMap;

		bool m_bUseFilter;
		CItemCriteria m_Filter;
	};

enum DeviceNames
	{
	devNone = -1,

	devFirstName = 0,

	devPrimaryWeapon = 0,
	devMissileWeapon = 1,

	devShields = 8,
	devDrive = 9,
	devCargo = 10,
	devReactor = 11,

	devNamesCount = 12
	};

struct DriveDesc
	{
	DWORD dwUNID;								//	UNID source (either ship class or device)
	Metric rMaxSpeed;							//	Max speed (Km/sec)
	int iThrust;								//	Thrust (GigaNewtons--gasp!)
	int iPowerUse;								//	Power used while thrusting (1/10 megawatt)

	DWORD fInertialess:1;						//	Inertialess drive
	DWORD dwSpare:31;
	};

struct ReactorDesc
	{
	int iMaxPower;								//	Maximum power output
	int iMaxFuel;								//	Maximum fuel space
	int iMinFuelLevel;							//	Min tech level of fuel (-1 if using fuelCriteria)
	int iMaxFuelLevel;							//	Max tech level of fuel (-1 if using fuelCriteria)
	int iPowerPerFuelUnit;						//	MW/10-tick per fuel unit

	DWORD fDamaged:1;							//	TRUE if damaged
	DWORD fEnhanced:1;							//	TRUE if enhanced
	DWORD dwSpare:30;
	};

class CInstalledArmor
	{
	public:
		inline EDamageResults AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void FinishInstall (CSpaceObject *pSource);
		inline CArmorClass *GetClass (void) const { return m_pArmorClass; }
		inline int GetHitPoints (void) const { return m_iHitPoints; }
		inline int GetMaxHP (CSpaceObject *pSource);
		inline const CItemEnhancement &GetMods (void) { return m_pItem->GetMods(); }
		inline int GetSect (void) const { return m_iSect; }
		inline int IncHitPoints (int iChange) { m_iHitPoints += iChange; return m_iHitPoints; }
		void Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iSect, bool bInCreate = false);
		inline bool IsComplete (void) const { return (m_fComplete ? true : false); }
		void SetComplete (CSpaceObject *pSource, bool bComplete = true);
		inline void SetHitPoints (int iHP) { m_iHitPoints = iHP; }
		inline void SetSect (int iSect) { m_iSect = iSect; }
		void ReadFromStream (CSpaceObject *pSource, int iSect, SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		CItem *m_pItem;								//	Item
		CArmorClass *m_pArmorClass;					//	Armor class used
		int m_iHitPoints;							//	Hit points left

		int m_iSect:16;								//	Armor section
		int m_iSpare:16;

		DWORD m_fComplete:1;						//	All armor segments the same
		DWORD m_fSpare:31;
	};

enum ArmorCompositionTypes
	{
	compUnknown = -1,

	compMetallic = 0,
	compCeramic = 1,
	compCarbide = 2,
	compNanoScale = 3,
	compQuantum = 4,
	compGravitic = 5,
	compDarkMatter = 6,
	};

ArmorCompositionTypes LoadArmorComposition (const CString &sString);

class CArmorClass : public CObject
	{
	public:
		enum ECachedHandlers
			{
			evtGetMaxHP					= 0,
			evtOnArmorDamage			= 1,

			evtCount					= 2,
			};

		EDamageResults AbsorbDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);
		int CalcAdjustedDamage (CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage);
		int CalcBalance (void);
		int CalcPowerUsed (CInstalledArmor *pArmor);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CArmorClass **retpArmor);
		bool FindDataField (const CString &sField, CString *retsValue);
		inline bool FindEventHandlerArmorClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		inline int GetBlindingDamageAdj (void) { return m_iBlindingDamageAdj; }
		inline int GetCompleteBonus (void) { return m_iArmorCompleteBonus; }
		inline int GetDamageAdj (DamageTypes iDamage) { return (iDamage == damageGeneric ? 100 : m_iDamageAdj[iDamage]); }
		inline int GetDeviceDamageAdj (void) { return m_iDeviceDamageAdj; }
		inline int GetEMPDamageAdj (void) { return m_iEMPDamageAdj; }
		inline CItemType *GetItemType (void) { return m_pItemType; }
		inline Metric GetMass (void) const;
		inline CString GetName (void);
		inline int GetInstallCost (void) { return m_iInstallCost; }
		int GetMaxHP (CItemCtx &ItemCtx);
		CString GetReference (CItemCtx &Ctx, int iVariant = -1);
		bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray);
		inline int GetRepairCost (void) { return m_iRepairCost; }
		inline int GetRepairTech (void) { return m_iRepairTech; }
		CString GetShortName (void);
		inline int GetStealth (void) const { return m_iStealth; }
		inline DWORD GetUNID (void);
		inline bool IsBlindingDamageImmune (CInstalledArmor *pArmor) { return (m_iBlindingDamageAdj == 0) || ((pArmor && pArmor->GetMods().IsBlindingImmune())); }
		inline bool IsDeviceDamageImmune (CInstalledArmor *pArmor) { return (m_iDeviceDamageAdj == 0) || ((pArmor && pArmor->GetMods().IsDeviceDamageImmune())); }
		inline bool IsDisintegrationImmune (CInstalledArmor *pArmor) { return (m_fDisintegrationImmune || (pArmor && pArmor->GetMods().IsDisintegrationImmune())); }
		inline bool IsEMPDamageImmune (CInstalledArmor *pArmor) { return (m_iEMPDamageAdj == 0) || ((pArmor && pArmor->GetMods().IsEMPImmune())); }
		inline bool IsRadiationImmune (CInstalledArmor *pArmor = NULL) { return (m_fRadiationImmune || (pArmor && pArmor->GetMods().IsRadiationImmune())); }
		bool IsReflective (CItemCtx &ItemCtx, const DamageDesc &Damage);
		inline bool IsShieldInterfering (CInstalledArmor *pArmor) { return (m_fShieldInterference || (pArmor && pArmor->GetMods().IsShieldInterfering())); }
		ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		void Update (CInstalledArmor *pArmor, CSpaceObject *pObj, int iTick, bool *retbModified);

		static int GetStdCost (int iLevel);
		static int GetStdDamageAdj (int iLevel, DamageTypes iDamage);
		static int GetStdEffectiveHP (int iLevel);
		static int GetStdHP (int iLevel);
		static int GetStdMass (int iLevel);

	private:
		CArmorClass (void);

		int GetDamageAdj (CItemEnhancement Mods, const DamageDesc &Damage);
		int FireGetMaxHP (CItemCtx &ItemCtx, int iMaxHP) const;
		void FireOnArmorDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);

		int m_iHitPoints;						//	Hit points for this armor class
		int m_iRepairCost;						//	Cost per HP to repair
		int m_iInstallCost;						//	Cost to install
		int m_iRepairTech;						//	Tech required to repair
		int m_iArmorCompleteBonus;				//	Extra HP if armor is complete
		int m_iStealth;							//	Stealth level
		int m_iPowerUse;						//	Power consumed (1/10 MWs)
		CRegenDesc m_Regen;						//	Regeneration desc
		CRegenDesc m_Decay;						//	Decay desc
		
		int m_iDamageAdjLevel;					//	Level to use for intrinsic damage adj
		int m_iDamageAdj[damageCount];			//	Adjustments for damage type
		DamageTypeSet m_Reflective;				//	Types of damage reflected
		int m_iEMPDamageAdj;					//	Adjust for EMP damage
		int m_iBlindingDamageAdj;				//	Adjust for blinding damage
		int m_iDeviceDamageAdj;					//	Adjust for device damage

		DWORD m_fPhotoRepair:1;					//	TRUE if repairs when near a star
		DWORD m_fRadiationImmune:1;				//	TRUE if immune to radiation
		DWORD m_fPhotoRecharge:1;				//	TRUE if refuels when near a star
		DWORD m_fShieldInterference:1;			//	TRUE if armor interferes with shields
		DWORD m_fDisintegrationImmune:1;		//	TRUE if immune to disintegration
		DWORD m_fSpare:27;

		CItemType *m_pItemType;					//	Item for this armor

		SEventHandlerDesc m_CachedEvents[evtCount];

	friend CObjectClass<CArmorClass>;
	};

class CDeviceClass : public CObject
	{
	public:
		enum CounterTypes
			{
			cntNone,							//	No counter
			cntTemperature,						//	Current device temperature (0-100)
			cntRecharge,						//	Current recharge level (0-100)
			cntCapacitor,						//	Current capacitor level (0-100)
			};

		enum DeviceNotificationTypes
			{
			statusDisruptionRepaired,

			failDamagedByDisruption,
			failDeviceHitByDamage,
			failDeviceHitByDisruption,
			failDeviceOverheat,
			failShieldFailure,
			failWeaponExplosion,
			failWeaponJammed,
			failWeaponMisfire,
			};

		enum LinkedFireOptions
			{
			lkfAlways =				0x0000001,	//	Linked to fire button
			lkfTargetInRange =		0x0000002,	//	Fire only if the target is in range
			lkfEnemyInRange =		0x0000004,	//	Fire only an enemy is in range
			};

		enum ECachedHandlers
			{
			evtGetOverlayType			= 0,

			evtCount					= 1,
			};

		enum Flags
			{
			flagNoPowerReference =		0x00000001,
			};

		CDeviceClass (IObjectClass *pClass) : CObject(pClass), m_pItemType(NULL) { }
		virtual ~CDeviceClass (void) { }

		ALERROR Bind (SDesignLoadCtx &Ctx);
		static bool FindAmmoDataField (CItemType *pItem, const CString &sField, CString *retsValue);
		inline CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		inline bool FindEventHandlerDeviceClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		CEnergyFieldType *FireGetOverlayType (CItemCtx &Ctx) const;
		inline CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		inline int GetDataFieldInteger (const CString &sField) { CString sValue; if (FindDataField(sField, &sValue)) return strToInt(sValue, 0, NULL); else return 0; }
		inline CItemType *GetItemType (void) { return m_pItemType; }
		inline int GetLevel (void) const;
		inline Metric GetMass (void);
		inline CString GetName (void);
		inline CEnergyFieldType *GetOverlayType (void) const { return m_pOverlayType; }
		CString GetReferencePower (CItemCtx &Ctx);
		inline int GetSlotsRequired (void) { return m_iSlots; }
		inline DWORD GetUNID (void);
		inline void LoadImages (void) { OnLoadImages(); }
		inline void MarkImages (void) { OnMarkImages(); }

		virtual bool AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx) { return false; }
		virtual bool AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon) { return false; }
		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) { return false; }
		virtual CWeaponClass *AsWeaponClass (void) { return NULL; }
		virtual int CalcFireSolution (CInstalledDevice *pDevice, CSpaceObject *pSource, CSpaceObject *pTarget) { return -1; }
		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual bool CanBeDamaged (void) { return true; }
		virtual bool CanBeDisabled (void) { return true; }
		virtual bool CanHitFriends (void) { return true; }
		virtual bool CanRotate (CItemCtx &Ctx) { return false; }
		virtual void Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual bool FindDataField (int iVariant, const CString &sField, CString *retsValue) { return false; }
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual int GetAmmoVariant (const CItemType *pItem) const { return -1; }
		virtual int GetCargoSpace (void) { return 0; }
		virtual ItemCategories GetCategory (void) = 0;
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL) { return 0; }
		virtual const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return NULL; }
		virtual int GetDamageType (CInstalledDevice *pDevice = NULL, int iVariant = -1) { return damageGeneric; }
		virtual int GetDefaultFireAngle (CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual bool GetDeviceEnhancementDesc (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon, SDeviceEnhancementDesc *retDesc) { return false; }
		virtual const DriveDesc *GetDriveDesc (CInstalledDevice *pDevice = NULL, CSpaceObject *pSource = NULL) { return NULL; }
		virtual DWORD GetLinkedFireOptions (CItemCtx &Ctx) { return 0; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) { return 0.0; }
		virtual int GetPowerRating (CItemCtx &Ctx) { return 0; }
		virtual const ReactorDesc *GetReactorDesc (CInstalledDevice *pDevice = NULL, CSpaceObject *pSource = NULL) { return NULL; }
		virtual CString GetReference (CItemCtx &Ctx, int iVariant = -1, DWORD dwFlags = 0);
		virtual bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const { return false; }
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, int iVariant, DamageTypes *retiDamage, CString *retsReference) const { return false; }
		virtual void GetSelectedVariantInfo (CSpaceObject *pSource, 
											 CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL) { }
		virtual void GetStatus (CInstalledDevice *pDevice, CShip *pShip, int *retiStatus, int *retiMaxStatus) { *retiStatus = 0; *retiMaxStatus = 0; }
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) { return 0; }
		virtual int GetWeaponEffectiveness (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) { return 0; }
		virtual bool IsAreaWeapon (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual bool IsAutomatedWeapon (void) { return false; }
		virtual bool IsExternal (void) { return (m_fExternal ? true : false); }
		virtual bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return false; }
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) { return true; }
		virtual bool IsWeaponAligned (CSpaceObject *pShip, CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) { return false; }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) { }
		virtual void OnUninstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) { }
		virtual void Recharge (CInstalledDevice *pDevice, CShip *pShip, int iStatus) { }
		virtual bool RequiresItems (void) { return false; }
		virtual void Reset (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual bool SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir = 1) { return false; }
		virtual bool ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) { }
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }

	protected:
		void InitDeviceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType);

		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual void OnLoadImages (void) { }
		virtual void OnMarkImages (void) { }

	private:
		CItemType *m_pItemType;					//	Item for device
		int m_iSlots;							//	Number of device slots required

		COverlayTypeRef m_pOverlayType;			//	Associated overlay (may be NULL)

		SEventHandlerDesc m_CachedEvents[evtCount];	//	Cached events

		DWORD m_fExternal:1;					//	Device is external
		DWORD m_dwSpare:31;
	};

class CRandomEnhancementGenerator
	{
	public:
		CRandomEnhancementGenerator (void) : m_iChance(0), m_dwMods(0), m_pCode(NULL) { }
		~CRandomEnhancementGenerator (void);

		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

		void EnhanceItem (CItem &Item);
		inline int GetChance (void) const { return m_iChance; }

	private:
		int m_iChance;
		DWORD m_dwMods;
		ICCItem *m_pCode;
	};

//	IItemGenerator

struct SItemAddCtx
	{
	SItemAddCtx (CItemListManipulator &theItemList) : 
			ItemList(theItemList),
			iLevel(1)
		{ }

	CItemListManipulator &ItemList;				//	Item list to add items to

	int iLevel;									//	Level to use for item create (for LevelTable)
	};

class IItemGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IItemGenerator **retpGenerator);
		static ALERROR CreateRandomItemTable (const CItemCriteria &Crit, 
											  const CString &sLevelFrequency,
											  IItemGenerator **retpGenerator);

		virtual ~IItemGenerator (void) { }
		virtual void AddItems (SItemAddCtx &Ctx) { }
		virtual IItemGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual CItemType *GetItemType (int iIndex) { return NULL; }
		virtual int GetItemTypeCount (void) { return 0; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
	};

//	IDeviceGenerator

struct SDeviceDesc
	{
	SDeviceDesc (void) :
			iPosAngle(0),
			iPosRadius(0),
			iPosZ(0),
			b3DPosition(false),
			bOmnidirectional(false),
			iMinFireArc(0),
			iMaxFireArc(0),
			bSecondary(false)
		{ }

	CItem Item;

	int iPosAngle;
	int iPosRadius;
	int iPosZ;
	bool b3DPosition;

	bool bOmnidirectional;
	int iMinFireArc;
	int iMaxFireArc;
	bool bSecondary;

	CItemList ExtraItems;
	};

class CDeviceDescList
	{
	public:
		CDeviceDescList (void);
		~CDeviceDescList (void);

		void AddDeviceDesc (const SDeviceDesc &Desc);
		inline int GetCount (void) const { return m_iCount; }
		inline CDeviceClass *GetDeviceClass (int iIndex) const;
		inline const SDeviceDesc &GetDeviceDesc (int iIndex) const { return m_pDesc[iIndex]; }
		CDeviceClass *GetNamedDevice (DeviceNames iDev) const;
		void RemoveAll (void);

	private:
		SDeviceDesc *m_pDesc;
		int m_iCount;
		int m_iAlloc;
	};

struct SDeviceGenerateCtx
	{
	SDeviceGenerateCtx (void) :
			iLevel(1),
			pRoot(NULL),
			pResult(NULL)
		{ }

	int iLevel;
	IDeviceGenerator *pRoot;

	CDeviceDescList *pResult;
	};

class IDeviceGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IDeviceGenerator **retpGenerator);

		virtual ~IDeviceGenerator (void) { }
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) { }
		virtual IDeviceGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

		virtual bool FindDefaultDesc (const CItem &Item, SDeviceDesc *retDesc) { return false; }
	};

//	Object properties

enum ScaleTypes
	{
	scaleStar,
	scaleWorld,
	scaleStructure,
	scaleShip,
	scaleFlotsam
	};

//	IShipGenerator

struct SShipCreateCtx
	{
	SShipCreateCtx (void) :
			pSystem(NULL),
			pGate(NULL),
			pBase(NULL),
			pBaseSovereign(NULL),
			pTarget(NULL),
			pEncounterInfo(NULL),
			pOverride(NULL),
			dwFlags(0)
		{ }

	CSystem *pSystem;							//	System to create ship in
	CSpaceObject *pGate;						//	Gate where ship will appear (may be NULL)
	CVector vPos;								//	Position where ship will appear (only if pGate is NULL)
	DiceRange PosSpread;						//	Distance from vPos (in light-seconds)
	CSpaceObject *pBase;						//	Base for this ship (may be NULL)
	CSovereign *pBaseSovereign;					//	Only if pBase is NULL
	CSpaceObject *pTarget;						//	Target for ship orders (may be NULL)
	CStationType *pEncounterInfo;				//	Encounter info (may be NULL)
	CDesignType *pOverride;						//	Override to apply to ships (may be NULL)

	DWORD dwFlags;								//	Flags

	CSpaceObjectList Result;					//	List of ships created

	enum Flags
		{
		SHIPS_FOR_STATION =			0x00000001,	//	Create ships at station creation time
		ATTACK_NEAREST_ENEMY =		0x00000002,	//	After all other orders, attack nearest enemy
		RETURN_RESULT =				0x00000004,	//	Add created ships to Result
		};
	};

class IShipGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IShipGenerator **retpGenerator);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IShipGenerator **retpGenerator);

		virtual ~IShipGenerator (void) { }
		virtual void CreateShips (SShipCreateCtx &Ctx) { }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual ALERROR ValidateForRandomEncounter (void) { return NOERROR; }
	};

//	Ship properties

enum ObjectComponentTypes
	{
	comArmor,
	comCargo,
	comShields,
	comWeapons,
	comDrive,
	comReactor,
	comDeviceCounter,					//	One or more devices need to show a counter
	};

struct SArmorImageDesc
	{
	CString sName;								//	Name of segment

	int xDest;									//	Coordinates of destination
	int yDest;
	int xHP;									//	Coordinates of HP display
	int yHP;
	int yName;									//	Coordinates of armor name
	int cxNameBreak;							//	Line break
	int xNameDestOffset;
	int yNameDestOffset;

	CObjectImageArray Image;					//	Image for armor damage
	};

struct SReactorImageDesc
	{
	CObjectImageArray ReactorImage;

	CObjectImageArray PowerLevelImage;
	int xPowerLevelImage;
	int yPowerLevelImage;

	CObjectImageArray FuelLevelImage;
	int xFuelLevelImage;
	int yFuelLevelImage;

	CObjectImageArray FuelLowLevelImage;

	RECT rcReactorText;
	RECT rcPowerLevelText;
	RECT rcFuelLevelText;
	};

struct SShieldImageDesc
	{
	CObjectImageArray Image;					//	Image for shields
	};

class CPlayerSettings
	{
	public:
		CPlayerSettings (void) : 
				m_pArmorDesc(NULL),
				m_pArmorDescInherited(NULL),
				m_pReactorDescInherited(NULL),
				m_pShieldDescInherited(NULL)
			{ }

		~CPlayerSettings (void) { CleanUp(); }

		CPlayerSettings &operator= (const CPlayerSettings &Source);

		ALERROR Bind (SDesignLoadCtx &Ctx, CShipClass *pClass);
		inline const SArmorImageDesc &GetArmorDesc (int iIndex) const { return (m_fHasArmorDesc ? m_pArmorDesc[iIndex] : m_pArmorDescInherited[iIndex]); }
		inline int GetArmorDescCount (void) const { return m_iArmorDescCount; }
		inline const CString &GetDesc (void) const { return m_sDesc; }
		inline DWORD GetLargeImage (void) const { return m_dwLargeImage; }
		inline const SReactorImageDesc &GetReactorDesc (void) const { return (m_fHasReactorDesc ? m_ReactorDesc : *m_pReactorDescInherited); }
		inline const SShieldImageDesc &GetShieldDesc (void) const { return (m_fHasShieldDesc ? m_ShieldDesc : *m_pShieldDescInherited); }
		inline const CDockScreenTypeRef &GetShipScreen (void) const { return m_pShipScreen; }
		inline const CCurrencyAndRange &GetStartingCredits (void) const { return m_StartingCredits; }
		inline const CString &GetStartNode (void) const { return m_sStartNode; }
		inline const CString &GetStartPos (void) const { return m_sStartPos; }
		inline bool HasAutopilot (void) const { return (m_fAutopilot ? true : false); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		inline bool IsDebugOnly (void) const { return (m_fDebug ? true : false); }
		inline bool IsInitialClass (void) const { return (m_fInitialClass ? true : false); }

		const SArmorImageDesc *GetArmorImageDescRaw (void) const { return (m_fHasArmorDesc ? m_pArmorDesc : NULL); }
		const SReactorImageDesc *GetReactorImageDescRaw (void) const { return (m_fHasReactorDesc ? &m_ReactorDesc : NULL); }
		const SShieldImageDesc *GetShieldImageDescRaw (void) const { return (m_fHasShieldDesc ? &m_ShieldDesc : NULL); }

	private:
		void CleanUp (void);
		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError);

		CString m_sDesc;							//	Description
		DWORD m_dwLargeImage;						//	UNID of large image

		//	Miscellaneous
		CCurrencyAndRange m_StartingCredits;		//	Starting credits
		CString m_sStartNode;						//	Starting node (may be blank)
		CString m_sStartPos;						//	Label of starting position (may be blank)
		CDockScreenTypeRef m_pShipScreen;			//	Ship screen

		//	Armor
		int m_iArmorDescCount;
		SArmorImageDesc *m_pArmorDesc;
		const SArmorImageDesc *m_pArmorDescInherited;

		//	Shields
		SShieldImageDesc m_ShieldDesc;
		const SShieldImageDesc *m_pShieldDescInherited;

		//	Reactor
		SReactorImageDesc m_ReactorDesc;
		const SReactorImageDesc *m_pReactorDescInherited;

		//	Flags
		DWORD m_fInitialClass:1;					//	Use ship class at game start
		DWORD m_fDebug:1;							//	Debug only
		DWORD m_fAutopilot:1;						//	Add autopilot to this class
		DWORD m_fHasArmorDesc:1;					//	TRUE if m_pArmorDesc initialized
		DWORD m_fHasReactorDesc:1;					//	TRUE if m_ReactorDesc initialized
		DWORD m_fHasShieldDesc:1;					//	TRUE if m_ShieldDesc initialized
		DWORD m_dwSpare:26;
	};

enum AICombatStyles
	{
	aicombatStandard,							//	Normal dogfighting
	aicombatStandOff,							//	Missile ship combat
	aicombatFlyby,								//	Maximize relative speed wrt target
	aicombatNoRetreat,							//	Do not turn away from target
	aicombatChase,								//	Get in position behind the target
	};

class CAISettings
	{
	public:
		CAISettings (void);

		inline AICombatStyles GetCombatStyle (void) const { return m_iCombatStyle; }
		inline int GetFireAccuracy (void) const { return m_iFireAccuracy; }
		inline int GetFireRangeAdj (void) const { return m_iFireRangeAdj; }
		inline int GetFireRateAdj (void) const { return m_iFireRateAdj; }
		inline Metric GetMinCombatSeparation (void) const { return m_rMinCombatSeparation; }
		inline int GetPerception (void) const { return m_iPerception; }
		CString GetValue (const CString &sSetting);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitToDefault (void);
		inline bool IsAggressor (void) const { return m_fAggressor; }
		inline bool IsNonCombatant (void) const { return m_fNonCombatant; }
		inline bool NoDogfights (void) const { return m_fNoDogfights; }
		inline bool NoFriendlyFire (void) const { return m_fNoFriendlyFire; }
		inline bool NoFriendlyFireCheck (void) const { return m_fNoFriendlyFireCheck; }
		inline bool NoOrderGiver (void) const { return m_fNoOrderGiver; }
		inline bool NoShieldRetreat (void) const { return m_fNoShieldRetreat; }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetMinCombatSeparation (Metric rValue) { m_rMinCombatSeparation = rValue; }
		CString SetValue (const CString &sSetting, const CString &sValue);
		void WriteToStream (IWriteStream *pStream);

	private:
		AICombatStyles m_iCombatStyle;			//	Combat style

		int m_iFireRateAdj;						//	Adjustment to weapon's fire rate (10 = normal; 20 = double delay)
		int m_iFireRangeAdj;					//	Adjustment to range (100 = normal; 50 = half range)
		int m_iFireAccuracy;					//	Percent chance of hitting
		int m_iPerception;						//	Perception

		Metric m_rMinCombatSeparation;			//	Min separation from other ships while in combat

		DWORD m_fNoShieldRetreat:1;				//	Ship does not retreat when shields go down
		DWORD m_fNoDogfights:1;					//	Don't turn ship to face target
		DWORD m_fNonCombatant:1;				//	Tries to stay out of trouble
		DWORD m_fNoFriendlyFire:1;				//	Cannot hit friends
		DWORD m_fAggressor:1;					//	Attack targets of opportunity even if they haven't attacked
		DWORD m_fNoFriendlyFireCheck:1;			//	Do not check to see if friends are in line of fire
		DWORD m_fNoOrderGiver:1;				//	Always treated as the decider

		DWORD m_dwSpare:25;
	};

enum ProgramTypes
	{
	progNOP,
	progShieldsDown,
	progReboot,
	progDisarm,

	progCustom,
	};

struct ProgramDesc
	{
	ProgramTypes iProgram;
	CString sProgramName;
	int iAILevel;

	//	Used for custom programs
	CEvalContext *pCtx;
	ICCItem *ProgramCode;
	};

//	Ship Controller

class IShipController
	{
	public:
		//	When adding a new order, also add to:
		//
		//	g_OrderTypes in Utilities.cpp

		enum OrderTypes
			{
			orderNone,					//	Nothing to do

			orderGuard,					//	pTarget = target to guard
			orderDock,					//	pTarget = station to dock with
			orderDestroyTarget,			//	pTarget = target to destroy; dwData = timer (0 == indefinitely)
			orderWait,					//	dwData = seconds to wait (0 == indefinitely)
			orderGate,					//	Gate out of the system (pTarget = optional gate)

			orderGateOnThreat,			//	Gate out of system if threatened
			orderGateOnStationDestroyed,//	Gate out if station is destroyed
			orderPatrol,				//	pTarget = target to orbit; dwData = radius (light-seconds)
			orderEscort,				//	pTarget = target to escort; dwData1 = angle; dwData2 = radius
			orderScavenge,				//	Scavenge for scraps

			orderFollowPlayerThroughGate,//	Follow the player through the nearest gate
			orderAttackNearestEnemy,	//	Attack the nearest enemy
			orderTradeRoute,			//	Moves from station to station
			orderWander,				//	Wander around system, avoiding enemies
			orderLoot,					//	pTarget = target to loot

			orderHold,					//	Stay in place; dwData = seconds (0 == indefinitely)
			orderMine,					//	Mine asteroids; pTarget = base
			orderWaitForPlayer,			//	Wait for player to return to the system
			orderDestroyPlayerOnReturn,	//	Wait to attack player when she returns
			orderFollow,				//	pTarget = target to follow (like escort, but no defense)

			orderNavPath,				//	dwData = nav path ID to follow
			orderGoTo,					//	Go to the given object (generally a marker)
			orderWaitForTarget,			//	Hold until pTarget is in LRS range (or dwData timer expires)
			orderWaitForEnemy,			//	Hold until any enemy is in LRS range (or dwData timer expires)
			orderBombard,				//	Hold and attack target from here; pTarget = target; dwData = time

			orderApproach,				//	Get within dwData ls of pTarget
			orderAimAtTarget,			//	Same as orderDestroyTarget, but we never fire
			orderOrbit,					//	pTarget = target to orbit; dwData1 = radius (light-seconds); dwData2 = timer
			orderHoldCourse,			//	dwData1 = course; dwData2 = distance (light-seconds)
			orderTurnTo,				//	dwData = angle to face

			orderDestroyTargetHold,		//	pTarget = target to destroy; dwData = timer
			orderAttackStation,			//	pTarget = station to destroy
			};

		enum ManeuverTypes
			{
			NoRotation,
			RotateLeft,
			RotateRight,

			ThrustLeft,
			ThrustRight
			};

		virtual ~IShipController (void) { }

		virtual void Behavior (void) { }
		virtual void CancelDocking (void) { }
		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual void DebugPaintInfo (CG16bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader) { return false; }
		virtual CString GetAISetting (const CString &sSetting) { return NULL_STR; }
		virtual CSpaceObject *GetBase (void) const { return NULL; }
		virtual int GetCombatPower (void) = 0;
		virtual CCurrencyBlock *GetCurrencyBlock (void) { return NULL; }
		virtual bool GetDeviceActivate (void) = 0;
		virtual CSpaceObject *GetEscortPrincipal (void) const { return NULL; }
		virtual int GetFireDelay (void) { return 0; }
		virtual int GetFireRateAdj (void) { return 10; }
		virtual ManeuverTypes GetManeuver (void) = 0;
		virtual CSpaceObject *GetOrderGiver (void) = 0;
		virtual GenomeTypes GetPlayerGenome (void) { return genomeUnknown; }
		virtual CString GetPlayerName (void) { return NULL_STR; }
		virtual bool GetReverseThrust (void) = 0;
		virtual bool GetStopThrust (void) = 0;
		virtual CSpaceObject *GetTarget (bool bNoAutoTarget = false) const { return NULL; }
		virtual bool GetThrust (void) = 0;
		virtual bool IsAngryAt (CSpaceObject *pObj) const { return false; }
		virtual bool IsPlayer (void) const { return false; }
		virtual bool IsPlayerWingman (void) const { return false; }
		virtual void ReadFromStream (SLoadCtx &Ctx, CShip *pShip) { ASSERT(false); }
		virtual CString SetAISetting (const CString &sSetting, const CString &sValue) { return NULL_STR; }
		virtual void SetCommandCode (ICCItem *pCode) { }
		virtual void SetManeuver (ManeuverTypes iManeuver) { }
		virtual void SetShipToControl (CShip *pShip) { }
		virtual void SetThrust (bool bThrust) { }
		virtual void SetPlayerWingman (bool bIsWingman) { }
		virtual void WriteToStream (IWriteStream *pStream) { ASSERT(false); }

		virtual void AddOrder(OrderTypes Order, CSpaceObject *pTarget, DWORD dwData, bool bAddBefore = false) { }
		virtual void CancelAllOrders (void) { }
		virtual void CancelCurrentOrder (void) { }
		virtual DWORD GetCurrentOrderData (void) { return 0; }
		virtual OrderTypes GetCurrentOrderEx (CSpaceObject **retpTarget = NULL, DWORD *retdwData = NULL) { return orderNone; }

		//	Events

		virtual void OnArmorRepaired (int iSection) { }
		virtual void OnAttacked (CSpaceObject *pAttacker, const DamageDesc &Damage) { }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual void OnDamaged (const CDamageSource &Cause, CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage) { }
		virtual void OnDeviceEnabledDisabled (int iDev, bool bEnabled) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockedObjChanged (CSpaceObject *pLocation) { }
		virtual void OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnFuelLowWarning (int iSeq) { }
		virtual void OnItemFired (const CItem &Item) { }
		virtual void OnItemInstalled (const CItem &Item) { }
		virtual void OnItemUninstalled (const CItem &Item) { }
		virtual void OnLifeSupportWarning (int iSecondsLeft) { }
		virtual void OnMessage (CSpaceObject *pSender, const CString &sMsg) { }
		virtual void OnNewSystem (void) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip) { }
		virtual void OnPlayerObj (CSpaceObject *pPlayer) { }
		virtual void OnProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) { }
		virtual void OnStatsChanged (void) { }
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnSystemLoaded (void) { }
		virtual void OnRadiationWarning (int iSecondsLeft) { }
		virtual void OnRadiationCleared (void) { }
		virtual void OnReactorOverloadWarning (int iSeq) { }
		virtual void OnWeaponStatusChanged (void) { }
		virtual void OnWreckCreated (CSpaceObject *pWreck) { }
		virtual void OnBlindnessChanged (bool bBlind, bool bNoMessage = false) { }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, int iEvent) { }
	};

struct SShipGeneratorCtx
	{
	SShipGeneratorCtx (void) :
			pItems(NULL),
			pOnCreate(NULL),
			iOrder(IShipController::orderNone),
			dwOrderData(0),
			pBase(NULL),
			pTarget(NULL),
			dwCreateFlags(0)
		{ }

	CString sName;								//	If not blank, use as name of ship
	DWORD dwNameFlags;							//	Name flags (only if sName is not blank)
	IItemGenerator *pItems;						//	Items to add to ship (may be NULL)
	CAttributeDataBlock InitialData;			//	Initial data
	ICCItem *pOnCreate;							//	Additional OnCreate code (may be NULL)
	DWORD dwCreateFlags;

	IShipController::OrderTypes iOrder;			//	Order for ship
	DWORD dwOrderData;

	CSpaceObject *pBase;						//	Base for ship (may be NULL)
	CSpaceObject *pTarget;						//	Target for ship (may be NULL)
	};

//	Effects

enum ParticlePaintStyles
	{
	paintPlain,
	paintFlame,
	paintSmoke,
	paintImage,
	paintLine,
	};

struct SParticlePaintDesc
	{
	SParticlePaintDesc (void)
		{
		iStyle = paintPlain;

		pImage = NULL;
		iVariants = 0;
		bRandomStartFrame = false;
		bDirectional = false;

		iMaxLifetime = -1;
		iMinWidth = 4;
		iMaxWidth = 4;

		wPrimaryColor = CG16bitImage::RGBValue(255, 255, 255);
		wSecondaryColor = CG16bitImage::RGBValue(0, 0, 0);
		}

	ParticlePaintStyles iStyle;

	CObjectImageArray *pImage;					//	Image for each particle
	int iVariants;								//	No of image variants
	bool bRandomStartFrame;						//	Start at a random animation
	bool bDirectional;							//	Different images for each direction

	int iMaxLifetime;							//	Max lifetime for particles
	int iMinWidth;								//	Min width of particle
	int iMaxWidth;								//	Max width of particle

	WORD wPrimaryColor;							//	Primary color 
	WORD wSecondaryColor;						//	Secondary color
	};

struct SEffectHitDesc
	{
	CSpaceObject *pObjHit;						//	Object that was hit by the effect
	CVector vHitPos;							//	Position hit
	int iHitStrength;							//	Number of particles hitting (or 0-100 hit strength)
	};

typedef TArray<SEffectHitDesc> CEffectHitResults;

struct SEffectUpdateCtx
	{
	SEffectUpdateCtx (void) : 
			pSystem(NULL),
			pObj(NULL),

			pDamageDesc(NULL),
			iDamageBonus(0),
			iCause(killedByDamage),
			bAutomatedWeapon(false),

			iTotalParticleCount(1),
			iWakePotential(0),

			bDestroy(false)
		{ }

	//	Object context
	CSystem *pSystem;							//	Current system
	CSpaceObject *pObj;							//	The object that the effect is part of

	//	Damage context
	CWeaponFireDesc *pDamageDesc;				//	Damage done by particles (may be NULL)
	int iDamageBonus;							//	Damage bonus
	DestructionTypes iCause;					//	Cause of damage
	bool bAutomatedWeapon;						//	TRUE if this is an automated attack
	CDamageSource Attacker;						//	Attacker

	//	Particle context
	int iTotalParticleCount;					//	Total particles
	int iWakePotential;							//	Objects influence particle motion (0-100)

	//	Outputs
	CEffectHitResults Hits;						//	Filled in with the objects that hit
	bool bDestroy;								//	Destroy the effect
	};

class IEffectPainter
	{
	public:
		void GetBounds (RECT *retRect);
		void GetBounds (const CVector &vPos, CVector *retvUR, CVector *retvLL);
		int GetInitialLifetime (void);
		inline void OnUpdate (void) { SEffectUpdateCtx Ctx; OnUpdate(Ctx); }
		inline void PlaySound (CSpaceObject *pSource);
		inline void ReadFromStream (SLoadCtx &Ctx) { OnReadFromStream(Ctx); }
		static CString ReadUNID (SLoadCtx &Ctx);
		static ALERROR ValidateClass (SLoadCtx &Ctx, const CString &sOriginalClass);
		void WriteToStream (IWriteStream *pStream);

		virtual ~IEffectPainter (void) { }
		virtual void Delete (void) { delete this; }
		virtual CEffectCreator *GetCreator (void) = 0;
		virtual int GetFadeLifetime (void) { return 0; }
		virtual int GetParticleCount (void) { return 1; }
		virtual bool GetParticlePaintDesc (SParticlePaintDesc *retDesc) { return false; }
		virtual Metric GetRadius (void) const;
		virtual void GetRect (RECT *retRect) const;
		virtual int GetVariants (void) const { return 1; }
		virtual void OnBeginFade (void) { }
		virtual void OnMove (bool *retbBoundsChanged = NULL) { if (retbBoundsChanged) *retbBoundsChanged = false; }
		virtual void OnUpdate (SEffectUpdateCtx &Ctx) { }
		virtual void Paint (CG16bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) = 0;
		virtual void PaintFade (CG16bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void PaintHit (CG16bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx) { }
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0) const { return false; }
		virtual void SetParamMetric (const CString &sParam, Metric rValue) { }
		virtual void SetPos (const CVector &vPos) { }
		virtual void SetVariants (int iVariants) { }

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }
	};

//	Topology Descriptors -------------------------------------------------------

const int INFINITE_NODE_DIST =					1000000;
const DWORD END_GAME_SYSTEM_UNID =				0x00ffffff;

class CTopologyNode
	{
	public:
		struct SDistanceTo
			{
			TArray<CString> AttribsRequired;
			TArray<CString> AttribsNotAllowed;

			CString sNodeID;

			int iMinDist;
			int iMaxDist;
			};

		struct SCriteriaCtx
			{
			SCriteriaCtx (void) :
					pTopology(NULL)
				{ }

			CTopology *pTopology;
			};

		struct SCriteria
			{
			int iChance;								//	Probability 0-100 of matching criteria
			int iMinStargates;							//	Match if >= this many stargates
			int iMaxStargates;							//	Match if <= this many stargates
			int iMinInterNodeDist;						//	Used by <DistributeNodes> (maybe move there)
			int iMaxInterNodeDist;
			TArray<CString> AttribsRequired;			//	Does not match if any of these attribs are missing
			TArray<CString> AttribsNotAllowed;			//	Does not match if any of these attribs are present
			TArray<SDistanceTo> DistanceTo;				//	Matches if node is within the proper distance of another node or nodes
			};

		CTopologyNode (const CString &sID, DWORD SystemUNID, CSystemMap *pMap);
		~CTopologyNode (void);
		static void CreateFromStream (SUniverseLoadCtx &Ctx, CTopologyNode **retpNode);

		void AddAttributes (const CString &sAttribs);
		ALERROR AddGateInt (const CString &sName, const CString &sDestNode, const CString &sEntryPoint);
		ALERROR AddStargate (const CString &sGateID, const CString &sDestNodeID, const CString &sDestGateID);
		ALERROR AddStargateConnection (CTopologyNode *pDestNode, bool bOneWay = false, const CString &sFromName = NULL_STR, const CString &sToName = NULL_STR);
		bool FindStargate (const CString &sName, CString *retsDestNode = NULL, CString *retsEntryPoint = NULL);
		bool FindStargateTo (const CString &sDestNode, CString *retsName = NULL, CString *retsDestGateID = NULL);
		CString FindStargateName (const CString &sDestNode, const CString &sEntryPoint);
		inline const CString &GetAttributes (void) const { return m_sAttributes; }
		inline const CString &GetData (const CString &sAttrib) const { return m_Data.GetData(sAttrib); }
		inline CSystemMap *GetDisplayPos (int *retxPos = NULL, int *retyPos = NULL);
		inline const CString &GetEndGameReason (void) { return m_sEndGameReason; }
		inline const CString &GetEpitaph (void) { return m_sEpitaph; }
		inline const CString &GetID (void) { return m_sID; }
		CTopologyNode *GetGateDest (const CString &sName, CString *retsEntryPoint = NULL);
		inline int GetLevel (void) { return m_iLevel; }
		inline int GetStargateCount (void) { return m_NamedGates.GetCount(); }
		CString GetStargate (int iIndex);
		CTopologyNode *GetStargateDest (int iIndex, CString *retsEntryPoint = NULL);
		inline CSystem *GetSystem (void) { return m_pSystem; }
		inline DWORD GetSystemDescUNID (void) { return m_SystemUNID; }
		inline DWORD GetSystemID (void) { return m_dwID; }
		inline const CString &GetSystemName (void) { return m_sName; }
		inline bool HasAttribute (const CString &sAttrib) { return ::HasModifier(m_sAttributes, sAttrib); }
		ALERROR InitFromAdditionalXML (CXMLElement *pDesc, CString *retsError);
		ALERROR InitFromAttributesXML (CXMLElement *pAttributes, CString *retsError);
		ALERROR InitFromSystemXML (CXMLElement *pSystem, CString *retsError);
		static bool IsCriteriaAll (const SCriteria &Crit);
		inline bool IsEndGame (void) const { return (m_SystemUNID == END_GAME_SYSTEM_UNID); }
		inline bool IsKnown (void) const { return m_bKnown; }
		inline bool IsMarked (void) const { return m_bMarked; }
		bool MatchesCriteria (SCriteriaCtx &Ctx, SCriteria &Crit);
		static ALERROR ParseCriteria (CXMLElement *pCrit, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteria (const CString &sCriteria, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteriaInt (const CString &sCriteria, SCriteria *retCrit);
		static ALERROR ParsePosition (const CString &sValue, int *retx, int *rety);
		static ALERROR ParseStargateString (const CString &sStargate, CString *retsNodeID, CString *retsGateName);
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetKnown (bool bKnown = true) { m_bKnown = bKnown; }
		inline void SetLevel (int iLevel) { m_iLevel = iLevel; }
		inline void SetMarked (bool bValue = true) { m_bMarked = bValue; }
		inline void SetName (const CString &sName) { m_sName = sName; }
		inline void SetPos (int xPos, int yPos) { m_xPos = xPos; m_yPos = yPos; }
		void SetStargateDest (const CString &sName, const CString &sDestNode, const CString &sEntryPoint);
		inline void SetSystem (CSystem *pSystem) { m_pSystem = pSystem; }
		inline void SetSystemID (DWORD dwID) { m_dwID = dwID; }
		inline void SetSystemUNID (DWORD dwUNID) { m_SystemUNID = dwUNID; }
		void WriteToStream (IWriteStream *pStream);

		inline void AddVariantLabel (const CString &sVariant) { m_VariantLabels.Insert(sVariant); }
		bool HasVariantLabel (const CString &sVariant);

	private:
		struct StarGateDesc
			{
			CString sDestNode;
			CString sDestEntryPoint;
			CTopologyNode *pDestNode;			//	Cached for efficiency (may be NULL)
			};

		CString GenerateStargateName (void);

		CString m_sID;							//	ID of node

		DWORD m_SystemUNID;						//	UNID of system type
		CString m_sName;						//	Name of system
		int m_iLevel;							//	Level of system

		CSystemMap *m_pMap;						//	May be NULL
		int m_xPos;								//	Position on map (cartessian)
		int m_yPos;

		CSymbolTable m_NamedGates;				//	Name to StarGateDesc

		CString m_sAttributes;					//	Attributes
		TArray<CString> m_VariantLabels;		//	Variant labels
		CString m_sEpitaph;						//	Epitaph if this is endgame node
		CString m_sEndGameReason;				//	End game reason if this is endgame node

		CAttributeDataBlock m_Data;				//	Opaque data

		CSystem *m_pSystem;						//	NULL if not yet created
		DWORD m_dwID;							//	ID of system instance

		bool m_bKnown;							//	TRUE if node is visible on galactic map
		bool m_bMarked;							//	Temp variable used during painting
	};

class CTopologyNodeList
	{
	public:
		inline CTopologyNode *operator [] (int iIndex) const { return m_List.GetAt(iIndex); }

		void Delete (CTopologyNode *pNode);
		inline void Delete (int iIndex) { m_List.Delete(iIndex); }
		inline void DeleteAll (void) { m_List.DeleteAll(); }
		ALERROR Filter (CTopologyNode::SCriteriaCtx &Ctx, CXMLElement *pCriteria, CTopologyNodeList *retList, CString *retsError);
		ALERROR Filter (CTopologyNode::SCriteriaCtx &Ctx, CTopologyNode::SCriteria &Crit, CTopologyNodeList *ioList);
		bool FindNode (CTopologyNode *pNode, int *retiIndex = NULL);
		bool FindNode (const CString &sID, int *retiIndex = NULL);
		inline CTopologyNode *GetAt (int iIndex) const { return m_List.GetAt(iIndex); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline void Insert (CTopologyNode *pNode) { m_List.Insert(pNode); }
		bool IsNodeInRangeOf (CTopologyNode *pNode, int iMin, int iMax, const TArray<CString> &AttribsRequired, const TArray<CString> &AttribsNotAllowed, CTopologyNodeList &Checked);
		void RestoreMarks (TArray<bool> &Saved);
		void SaveAndSetMarks (bool bMark, TArray<bool> *retSaved);
		inline void Shuffle (void) { m_List.Shuffle(); }

	private:
		TArray<CTopologyNode *> m_List;
	};

enum ENodeDescTypes
	{
	ndNode =			0x01,					//	A single node
	ndNodeTable =		0x02,					//	A table of descriptors (each of which can only appear once)
	ndFragment =		0x03,					//	A topology of nodes (used as fragment)
	ndNetwork =			0x04,					//	A network of nodes (used as fragment)
	ndRandom =			0x05,					//	Randomly generated network
	};

class CTopologyDesc
	{
	public:
		CTopologyDesc (void);
		~CTopologyDesc (void);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		CString GetAttributes (void);
		inline CXMLElement *GetDesc (void) const { return m_pDesc; }
		inline CEffectCreator *GetLabelEffect (void) const { return m_pLabelEffect; }
		inline CEffectCreator *GetMapEffect (void) const { return m_pMapEffect; }
		inline const CString &GetID (void) const { return m_sID; }
		void GetPos (int *retx, int *rety);
		CXMLElement *GetSystem (void);
		inline CTopologyDesc *GetTopologyDesc (int iIndex);
		inline int GetTopologyDescCount (void);
		inline CTopologyDescTable *GetTopologyDescTable (void) { return m_pDescList; }
		inline ENodeDescTypes GetType (void) const { return m_iType; }
		inline bool IsAbsoluteNode (void) const { return (*m_sID.GetASCIIZPointer() != '+'); }
		bool IsEndGameNode (CString *retsEpitaph = NULL, CString *retsReason = NULL) const;
		inline bool IsRootNode (void) const { return ((m_dwFlags & FLAG_IS_ROOT_NODE) ? true : false); }
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pXMLDesc, const CString &sParentUNID);
		inline void SetRootNode (void) { m_dwFlags |= FLAG_IS_ROOT_NODE; }

	private:
		enum Flags
			{
			FLAG_IS_ROOT_NODE = 0x00000001,
			};

		CString m_sID;							//	ID of node
		ENodeDescTypes m_iType;					//	Type of node
		CXMLElement *m_pDesc;					//	XML for node definition
		DWORD m_dwFlags;

		CEffectCreatorRef m_pLabelEffect;		//	Effect to paint on label layer
		CEffectCreatorRef m_pMapEffect;			//	Effect to paint on galactic map

		CTopologyDescTable *m_pDescList;		//	Some node types (e.g., ndNetwork) have sub-nodes.
												//		This is a table of all subnodes
	};

class CTopologyDescTable
	{
	public:
		CTopologyDescTable (void);
		~CTopologyDescTable (void);

		ALERROR AddRootNode (SDesignLoadCtx &Ctx, const CString &sNodeID);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		inline void DeleteIDMap (void) { delete m_pIDToDesc; m_pIDToDesc = NULL; }
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		CTopologyDesc *FindTopologyDesc (const CString &sID);
		CXMLElement *FindTopologyDescXML (const CString &sNodeID);
		CTopologyDesc *FindRootNodeDesc (const CString &sID);
		CXMLElement *FindRootNodeDescXML (const CString &sNodeID);
		inline const CString &GetFirstNodeID (void) { return m_sFirstNode; }
		inline int GetRootNodeCount (void) { return m_RootNodes.GetCount(); }
		inline CTopologyDesc *GetRootNodeDesc (int iIndex) { return m_RootNodes[iIndex]; }
		inline CXMLElement *GetRootNodeDescXML (int iIndex) { return m_RootNodes[iIndex]->GetDesc(); }
		inline CTopologyDesc *GetTopologyDesc (int iIndex) { return m_Table[iIndex]; }
		inline int GetTopologyDescCount (void) { return m_Table.GetCount(); }
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sParentUNID, bool bAddFirstAsRoot = false);
		ALERROR LoadNodeFromXML (SDesignLoadCtx &Ctx, CXMLElement *pNode, const CString &sParentUNID, CTopologyDesc **retpNode = NULL);

	private:
		void InitIDMap (void);

		TArray<CTopologyDesc *> m_Table;
		TMap<CString, CTopologyDesc *> *m_pIDToDesc;
		TArray<CTopologyDesc *> m_RootNodes;

		CString m_sFirstNode;						//	Node where player starts (if not specified elsewhere)
	};

inline CTopologyDesc *CTopologyDesc::GetTopologyDesc (int iIndex) { return (m_pDescList ? m_pDescList->GetTopologyDesc(iIndex) : NULL); }
inline int CTopologyDesc::GetTopologyDescCount (void) { return (m_pDescList ? m_pDescList->GetTopologyDescCount() : 0); }

struct STopologyCreateCtx
	{
	STopologyCreateCtx (void) :
			pMap(NULL),
			pTopologyTable(NULL),
			pFragmentTable(NULL),
			pNodesAdded(NULL),
			bInFragment(false),
			xOffset(0),
			yOffset(0),
			iRotation(0)
		{ }

	CSystemMap *pMap;								//	Map that we're currently processing
	CTopologyDescTable *pTopologyTable;
	CTopologyDescTable *pFragmentTable;
	CTopologyNodeList *pNodesAdded;					//	Output of nodes added

	bool bInFragment;
	CString sFragmentPrefix;
	int xOffset;
	int yOffset;
	int iRotation;
	CString sFragmentExitID;
	CString sFragmentExitGate;
	CString sFragmentAttributes;

	CString sError;
	};

//	Games and Records ----------------------------------------------------------

struct SBasicGameStats
	{
	int iSystemsVisited;
	int iEnemiesDestroyed;
	DWORD dwBestEnemyDestroyed;
	int iBestEnemyDestroyedCount;
	};

class CGameRecord
	{
	public:
		enum EDescParts
			{
			descCharacter =				0x00000001,
			descShip =					0x00000002,
			descEpitaph =				0x00000004,
			descDate =					0x00000008,
			descPlayTime =				0x00000010,
			descResurrectCount =		0x00000020,

			descAll =					0x0000003F,
			};

		enum Flags
			{
			FLAG_OMIT_WAS =				0x00000001,	//	Remove leading "was" from "was destroyed..."
			};

		CGameRecord (void);

		CString GetAdventureID (void) const;
		inline DWORD GetAdventureUNID (void) const { return m_dwAdventure; }
		CString GetDescription (DWORD dwParts = descAll) const;
		CString GetEndGameEpitaph (DWORD dwFlags = 0) const;
		inline const CString &GetEndGameReason (void) const { return m_sEndGameReason; }
		inline const CString &GetGameID (void) const { return m_sGameID; }
		inline GenomeTypes GetPlayerGenome (void) const { return m_iGenome; }
		inline const CString &GetPlayerName (void) const { return m_sName; }
		inline CString GetPlayTimeString (void) const { return m_Duration.Format(NULL_STR); }
		inline int GetResurrectCount (void) const { return m_iResurrectCount; }
		inline int GetScore (void) const { return m_iScore; }
		inline CString GetShipClass (void) const { return m_sShipClass; }
		inline const CString &GetUsername (void) const { return m_sUsername; }
		ALERROR InitFromJSON (const CJSONValue &Value);
		ALERROR InitFromXML (CXMLElement *pDesc);
		inline bool IsDebug (void) const { return m_bDebugGame; }
		static GenomeTypes LoadGenome (const CString &sAttrib);
		void SaveToJSON (CJSONValue *retOutput) const;
		inline void SetAdventureUNID (DWORD dwUNID) { m_dwAdventure = dwUNID; }
		inline void SetDebug (bool bDebug = true) { m_bDebugGame = bDebug; }
		inline void SetEndGameEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetExtensions (const TArray<DWORD> &Extensions) { m_Extensions = Extensions; }
		inline void SetGameID (const CString &sGameID) { m_sGameID = sGameID; }
		inline void SetPlayerGenome (GenomeTypes iGenome) { m_iGenome = iGenome; }
		inline void SetPlayerName (const CString &sName) { m_sName = sName; }
		inline void SetPlayTime (const CTimeSpan &Time) { m_Duration = Time; }
		inline void SetResurrectCount (int iCount) { m_iResurrectCount = iCount; }
		inline void SetScore (int iScore) { m_iScore = iScore; }
		void SetShipClass (DWORD dwUNID);
		void SetSystem (CSystem *pSystem);
		inline void SetUsername (const CString &sUsername) { m_sUsername = sUsername; }
		ALERROR WriteToXML (IWriteStream &Stream);

	private:
		//	Basic
		CString m_sUsername;					//	Username
		CString m_sGameID;						//	ID of game played
		DWORD m_dwAdventure;					//	UNID of adventure
		TArray<DWORD> m_Extensions;				//	UNID of included extensions

		CString m_sName;						//	Character name
		GenomeTypes m_iGenome;					//	Character genome

		DWORD m_dwShipClass;					//	Ship class UNID
		CString m_sShipClass;					//	Ship class
		CString m_sSystem;						//	NodeID of current system
		CString m_sSystemName;					//	Name of current system

		CTimeDate m_CreatedOn;					//	Game created on this date (set by server)
		CTimeDate m_ReportedOn;					//	Time/date of latest report (set by server)
		CTimeSpan m_Duration;					//	Time played
		bool m_bDebugGame;						//	If TRUE, this is a debug game

		//	Stats
		int m_iScore;
		int m_iResurrectCount;

		//	End Game
		CString m_sEndGameReason;
		CString m_sEpitaph;
	};

class CGameStats
	{
	public:
		inline void DeleteAll (void) { m_Stats.DeleteAll(); }
		inline int GetCount (void) const { return m_Stats.GetCount(); }
		inline const CString &GetDefaultSectionName (void) const { return m_sDefaultSectionName; }
		void GetEntry (int iIndex, CString *retsStatName, CString *retsStatValue, CString *retsSection) const;
		void Insert (const CString &sStatName, const CString &sStatValue = NULL_STR, const CString &sSection = NULL_STR, const CString &sSortKey = NULL_STR);
		ALERROR LoadFromStream (IReadStream *pStream);
		void SaveToJSON (CJSONValue *retOutput) const;
		ALERROR SaveToStream (IWriteStream *pStream) const;
		inline void SetDefaultSectionName (const CString &sName) { m_sDefaultSectionName = sName; }
		inline void Sort (void) { m_Stats.Sort(); }
		void TakeHandoff (CGameStats &Source);
		ALERROR WriteAsText (IWriteStream *pOutput) const;

	private:
		struct SStat
			{
			//	Needed for TArray's sort method
			bool operator > (const SStat &Right) const { return (strCompare(sSortKey, Right.sSortKey) == 1); }
			bool operator < (const SStat &Right) const { return (strCompare(sSortKey, Right.sSortKey) == -1); }

			CString sSortKey;
			CString sStatName;
			CString sStatValue;
			};

		void ParseSortKey (const CString &sSortKey, CString *retsSection, CString *retsSectionSortKey) const;

		TArray<SStat> m_Stats;
		CString m_sDefaultSectionName;
	};

class CAdventureRecord
	{
	public:
		enum ESpecialScoreIDs
			{
			personalBest =			10000,
			mostRecent =			10001,

			specialIDFirst =		10000,
			specialIDCount =		2,
			};

		CAdventureRecord (void) : m_dwAdventure(0) { }
		CAdventureRecord (DWORD dwAdventure, int iHighScoreCount);

		bool FindRecordByGameID (const CString &sGameID, DWORD *retdwID = NULL) const;
		CString GetAdventureName (void) const;
		DWORD GetAdventureUNID (void) const { return m_dwAdventure; }
		inline int GetHighScoreCount (void) const { return m_HighScores.GetCount(); }
		CGameRecord &GetRecordAt (DWORD dwID);
		void Init (DWORD dwAdventure);
		CGameRecord &InsertHighScore (void) { return *m_HighScores.Insert(); }
		inline bool IsSpecialID (DWORD dwID) const { return (dwID >= specialIDFirst && dwID < (specialIDFirst + specialIDCount)); }

	private:
		DWORD m_dwAdventure;
		TArray<CGameRecord> m_HighScores;
		CGameRecord m_Special[specialIDCount];
	};

class CUserProfile
	{
	public:
		CUserProfile (void) { }

		bool FindAdventureRecord (DWORD dwAdventure, int *retiIndex = NULL);
		inline CAdventureRecord &GetAdventureRecord (int iIndex) { return m_Records[iIndex]; }
		inline int GetAdventureRecordCount (void) { return m_Records.GetCount(); }
		void Init (const CString &sUsername);
		CAdventureRecord &InsertAdventureRecord (DWORD dwAdventure);

	private:
		CString m_sUsername;
		TArray<CAdventureRecord> m_Records;
	};

//	CItemType -----------------------------------------------------------------

class CItemCtx
	{
	public:
		CItemCtx (const CItem *pItem = NULL, CSpaceObject *pSource = NULL) : m_pItem(pItem), m_pSource(pSource), m_pArmor(NULL), m_pDevice(NULL) { }
		CItemCtx (const CItem *pItem, CSpaceObject *pSource, CInstalledArmor *pArmor) : m_pItem(pItem), m_pSource(pSource), m_pArmor(pArmor), m_pDevice(NULL) { }
		CItemCtx (const CItem *pItem, CSpaceObject *pSource, CInstalledDevice *pDevice) : m_pItem(pItem), m_pSource(pSource), m_pArmor(NULL), m_pDevice(pDevice) { }
		CItemCtx (CSpaceObject *pSource, CInstalledArmor *pArmor) : m_pItem(NULL), m_pSource(pSource), m_pArmor(pArmor), m_pDevice(NULL) { }
		CItemCtx (CSpaceObject *pSource, CInstalledDevice *pDevice) : m_pItem(NULL), m_pSource(pSource), m_pArmor(NULL), m_pDevice(pDevice) { }

		ICCItem *CreateItemVariable (CCodeChain &CC);
		CInstalledArmor *GetArmor (void);
		CInstalledDevice *GetDevice (void);
		CDeviceClass *GetDeviceClass (void);
		const CItem &GetItem (void);
		const CItemEnhancement &GetMods (void);
		inline CSpaceObject *GetSource (void) { return m_pSource; }

	private:
		const CItem *GetItemPointer (void);

		const CItem *m_pItem;					//	The item
		CItem m_Item;							//	A cached item, if we need to cons one up.
		CSpaceObject *m_pSource;				//	Where the item is installed (may be NULL)
		CInstalledArmor *m_pArmor;				//	Installation structure (may be NULL)
		CInstalledDevice *m_pDevice;			//	Installation structure (may be NULL)
	};

class CItemType : public CDesignType
	{
	public:
		enum Flags
			{
			//	Flags for GetReference, 
			//	GetReferenceDamageAdj, 
			//	and GetReferenceDamateType
			FLAG_ACTUAL_ITEM =			0x00000001,	//	Ignore Unknown flag
			};

		enum ECachedHandlers
			{
			evtGetName					= 0,
			evtGetTradePrice			= 1,
			evtOnInstall				= 2,
			evtOnEnabled				= 3,
			evtOnRefuel					= 4,

			evtCount					= 5,
			};

		CItemType (void);
		virtual ~CItemType (void);

		inline bool AreChargesValued (void) const { return (m_fValueCharges ? true : false); }
		inline void ClearKnown (void) { m_fKnown = false; }
		inline void ClearShowReference (void) { m_fReference = false; }
		inline bool FindEventHandlerItemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		int GetApparentLevel (void) const;
		CDeviceClass *GetAmmoLauncher (int *retiVariant = NULL) const;
		inline CArmorClass *GetArmorClass (void) const { return m_pArmor; }
		ItemCategories GetCategory (void) const;
		inline int GetCharges (void) const { return (m_fInstanceData ? m_InitDataValue.Roll() : 0); }
		inline CEconomyType *GetCurrencyType (void) const { return m_iValue.GetCurrencyType(); }
		inline const CString &GetData (void) const { return m_sData; }
		const CString &GetDesc (void) const;
		inline CDeviceClass *GetDeviceClass (void) const { return m_pDevice; }
		inline int GetFrequency (void) const { return m_Frequency; }
		int GetFrequencyByLevel (int iLevel);
		inline const CObjectImageArray &GetImage (void) { return m_Image; }
		int GetInstallCost (void) const;
		inline const DiceRange &GetNumberAppearing (void) const { return m_NumberAppearing; }
		inline Metric GetMass (void) { return m_iMass / 1000.0; }
		inline int GetMassKg (void) { return m_iMass; }
		inline int GetMaxCharges (void) const { return (m_fInstanceData ? m_InitDataValue.GetMaxValue() : 0); }
		inline DWORD GetModCode (void) const { return m_dwModCode; }
		CString GetName (DWORD *retdwFlags, bool bActualName = false) const;
		CString GetNounPhrase (DWORD dwFlags = 0) const;
		CString GetReference (CItemCtx &Ctx, int iVariant = -1, DWORD dwFlags = 0) const;
		CString GetSortName (void) const;
		inline CItemType *GetUnknownType (void) { return m_pUnknownType; }
		inline ICCItem *GetUseCode (void) const { return m_pUseCode; }
		inline const CString &GetUseKey (void) const { return m_sUseKey; }
		inline CXMLElement *GetUseScreen (void) const;
		inline CDesignType *GetUseScreen (CString *retsName);
		int GetValue (bool bActual = false) const;
		inline bool HasOnRefuelCode (void) const { return FindEventHandlerItemType(evtOnRefuel); }
		inline bool HasOnInstallCode (void) const { return FindEventHandlerItemType(evtOnInstall); }
		void InitRandomNames (void);
		inline bool IsArmor (void) const { return (m_pArmor != NULL); }
		inline bool IsDevice (void) const { return (m_pDevice != NULL); }
		inline bool IsKnown (void) const { return (m_fKnown ? true : false); }
		bool IsFuel (void) const;
		bool IsMissile (void) const;
		inline bool IsUsable (void) const { return ((m_pUseCode != NULL) || (m_pUseScreen != NULL)); }
		inline bool IsUsableInCockpit (void) const { return (m_pUseCode != NULL); }
		inline bool IsUsableOnlyIfInstalled (void) const { return (m_fUseInstalled ? true : false); }
		inline bool IsUsableOnlyIfUninstalled (void) const { return (m_fUseUninstalled ? true : false); }
		inline void SetKnown (void) { m_fKnown = true; }
		inline void SetShowReference (void) { m_fReference = true; }
		inline bool ShowReference (void) const { return (m_fReference ? true : false); }

		//	CDesignType overrides
		static CItemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designItemType) ? (CItemType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual int GetLevel (void) const { return m_iLevel; }
		virtual DesignTypes GetType (void) { return designItemType; }
		virtual bool IsVirtual (void) const { return (m_fVirtual ? true : false); }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID);
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx);
		virtual void OnReinit (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		static ALERROR ComposeError (const CString &sName, const CString &sError, CString *retsError);
		CString GetUnknownName (int iIndex, DWORD *retdwFlags = NULL);

		CString m_sName;						//	Full name of item
		CString m_sUnknownName;					//	Name of item when unknown (may be NULL)
		CString m_sSortName;					//	Name to sort by

		int m_iLevel;							//	Level of item
		CCurrencyAndValue m_iValue;				//	Value in some currency
		int m_iMass;							//	Mass in kilograms
		FrequencyTypes m_Frequency;				//	Frequency
		DiceRange m_NumberAppearing;			//	Number appearing

		CObjectImageArray m_Image;				//	Image of item
		CString m_sDescription;					//	Description of item
		CItemTypeRef m_pUnknownType;			//	Type to show if not known
		TArray<CString> m_UnknownNames;			//	List of unknown names (if we are the unknown item placeholder)
		DiceRange m_InitDataValue;				//	Initial data value

		//	Events
		SEventHandlerDesc m_CachedEvents[evtCount];

		//	Usable items
		CDockScreenTypeRef m_pUseScreen;		//	Screen shown when used (may be NULL)
		ICCItem *m_pUseCode;					//	Code when using item from cockpit
		CString m_sUseKey;						//	Hotkey to invoke usage
		DWORD m_dwModCode;						//	Mod code conveyed to items we enhance

		//	Armor
		CArmorClass *m_pArmor;					//	Armor properties (may be NULL)

		//	Devices
		CDeviceClass *m_pDevice;				//	Device properties (may be NULL)

		DWORD m_fFirstPlural:1;					//	Pluralize first word
		DWORD m_fSecondPlural:1;				//	Pluralize second word
		DWORD m_fESPlural:1;					//	Pluralize by adding "es"
		DWORD m_fRandomDamaged:1;				//	Randomly damaged when found
		DWORD m_fKnown:1;						//	Is this type known?
		DWORD m_fReference:1;					//	Does this type show reference info?
		DWORD m_fDefaultReference:1;			//	TRUE if this shows reference by default
		DWORD m_fInstanceData:1;				//	TRUE if we need to set instance data at create time
		DWORD m_fReverseArticle:1;				//	"a" instead of "an" or vice versa
		DWORD m_fVirtual:1;						//	TRUE if this is a virtual item needed for a weapon that invokes
		DWORD m_fUseInstalled:1;				//	If TRUE, item can only be used when installed
		DWORD m_fValueCharges:1;				//	TRUE if value should be adjusted based on charges
		DWORD m_fUseUninstalled:1;				//	If TRUE, item can only be used when uninstalled
		DWORD m_dwSpare:19;

		CString m_sData;						//	Category-specific data
	};

//	CItemTable ----------------------------------------------------------------

class CItemTable : public CDesignType
	{
	public:
		CItemTable (void);
		virtual ~CItemTable (void);

		inline void AddItems (SItemAddCtx &Ctx) { if (m_pGenerator) m_pGenerator->AddItems(Ctx); }
		inline IItemGenerator *GetGenerator (void) { return m_pGenerator; }

		//	CDesignType overrides
		static CItemTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designItemTable) ? (CItemTable *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designItemTable; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		IItemGenerator *m_pGenerator;
	};

//	CShipClass ----------------------------------------------------------------

class CShipClass : public CDesignType
	{
	public:
		enum VitalSections
			{
			sectNonCritical	= 0x00000000,		//	Ship is not automatically
												//	destroyed (chance of being
												//	destroyed based on the power
												//	of the blast relative to the
												//	original armor HPs)

			//	NOTE: These must match the order in CShipClass.cpp (ParseNonCritical)

			sectDevice0		= 0x00000001,
			sectDevice1		= 0x00000002,
			sectDevice2		= 0x00000004,
			sectDevice3		= 0x00000008,
			sectDevice4		= 0x00000010,
			sectDevice5		= 0x00000020,
			sectDevice6		= 0x00000040,
			sectDevice7		= 0x00000080,
			sectDeviceMask	= 0x000000ff,

			sectManeuver	= 0x00000100,		//	Maneuvering damaged (quarter speed turn)
			sectDrive		= 0x00000200,		//	Drive damaged (half-speed)
			sectScanners	= 0x00000400,		//	Unable to target
			sectTactical	= 0x00000800,		//	Unable to fire weapons
			sectCargo		= 0x00001000,		//	Random cargo destroyed

			sectCritical	= 0x00010000,		//	Ship destroyed
			};

		struct HullSection
			{
			int iStartAt;						//	Start of section in degrees
			int iSpan;							//	Size of section in degrees
			CArmorClassRef pArmor;				//	Type of armor for hull
			CRandomEnhancementGenerator Enhanced;//	Mods
			DWORD dwAreaSet;					//	Areas that this section protects
			};

		CShipClass (void);
		virtual ~CShipClass (void);

		inline int Angle2Direction (int iAngle) const
			{ return ((m_iRotationRange - (iAngle / m_iRotationAngle)) + (m_iRotationRange / 4)) % m_iRotationRange; }
		inline int AlignToRotationAngle (int iAngle) const
			{ return (((m_iRotationRange - Angle2Direction(iAngle)) + (m_iRotationRange / 4)) * m_iRotationAngle) % 360; }
		Metric CalcMass (const CDeviceDescList &Devices);
		int CalcScore (void);
		void CreateEmptyWreck (CSystem *pSystem, CShip *pShip, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpWreck);
		void CreateExplosion (CShip *pShip, CSpaceObject *pWreck = NULL);
		void CreateWreck (CShip *pShip, CSpaceObject **retpWreck = NULL);
		inline bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) { return (m_pDevices ? m_pDevices->FindDefaultDesc(Item, retDesc) : false); }
		void GenerateDevices (int iLevel, CDeviceDescList &Devices);
		CString GenerateShipName (DWORD *retdwFlags);
		inline const CAISettings &GetAISettings (void) { return m_AISettings; }
		const SArmorImageDesc *GetArmorDescInherited (void);
		inline int GetCargoSpace (void) { return m_iCargoSpace; }
		inline int GetCyberDefenseLevel (void) { return m_iCyberDefenseLevel; }
		inline DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		inline int GetDockingPortCount (void) { return m_iDockingPortsCount; }
		inline CVector *GetDockingPortPositions (void) { return m_DockingPorts; }
		void GetDriveDesc (DriveDesc *retDriveDesc);
		IShipGenerator *GetEscorts (void) { return m_pEscorts; }
		CWeaponFireDesc *GetExplosionType (void) { return m_pExplosionType; }
		inline CXMLElement *GetFirstDockScreen (void) { return m_pDefaultScreen.GetDesc(); }
		inline CDesignType *GetFirstDockScreen (CString *retsName) { return m_pDefaultScreen.GetDockScreen(this, retsName); }
		inline const DriveDesc *GetHullDriveDesc (void) { return &m_DriveDesc; }
		inline int GetHullMass (void) { return m_iMass; }
		inline int GetHullSectionCount (void) { return m_Hull.GetCount(); }
		inline HullSection *GetHullSection (int iIndex) { return (HullSection *)m_Hull.GetStruct(iIndex); }
		int GetHullSectionAtAngle (int iAngle);
		inline const CObjectImageArray &GetImage (void) { return m_Image; }
		inline const CObjectImageArray &GetImageSmall (void) { return m_Image; }
		inline int GetManeuverability (void) { return m_iManeuverability; }
		inline int GetManeuverDelay (void) { return m_iManeuverDelay; }
		inline int GetMaxArmorMass (void) const { return m_iMaxArmorMass; }
		inline int GetMaxCargoSpace (void) const { return m_iMaxCargoSpace; }
		inline int GetMaxDevices (void) const { return m_iMaxDevices; }
		inline int GetMaxNonWeapons (void) const { return m_iMaxNonWeapons; }
		inline int GetMaxReactorPower (void) const { return m_iMaxReactorPower; }
		int GetMaxStructuralHitPoints (void) const;
		inline int GetMaxWeapons (void) const { return m_iMaxWeapons; }
		CString GetName (DWORD *retdwFlags = NULL);
		CString GetNounPhrase (DWORD dwFlags);
		inline const CPlayerSettings *GetPlayerSettings (void) const { return m_pPlayerSettings; }
		inline IItemGenerator *GetRandomItemTable (void) { return m_pItems; }
		inline const ReactorDesc *GetReactorDesc (void) { return &m_ReactorDesc; }
		const SReactorImageDesc *GetReactorDescInherited (void);
		inline int GetRotationAngle (void) { return m_iRotationAngle; }
		inline int GetRotationRange (void) { return m_iRotationRange; }
		inline int GetScore (void) { return m_iScore; }
		const SShieldImageDesc *GetShieldDescInherited (void);
		inline DWORD GetShipNameFlags (void) { return m_dwShipNameFlags; }
		CString GetShortName (void);
		inline const CString &GetClassName (void) { return m_sName; }
		inline const CString &GetManufacturerName (void) { return m_sManufacturer; }
		inline const CString &GetTypeName (void) { return m_sTypeName; }
		inline int GetWreckChance (void) { return m_iLeavesWreck; }
		CObjectImageArray &GetWreckImage (void) { if (m_WreckImage.IsEmpty()) CreateWreckImage(); return m_WreckImage; }
		void GetWreckImage (CObjectImageArray *retWreckImage);
		int GetWreckImageVariants (void);
		inline bool HasDockingPorts (void) { return (m_fHasDockingPorts ? true : false); }
		inline bool HasOnAttackedByPlayerEvent (void) const { return (m_fHasOnAttackedByPlayerEvent ? true : false); }
		inline bool HasOnObjDockedEvent (void) const { return (m_fHasOnObjDockedEvent ? true : false); }
		inline bool HasOnOrderChangedEvent (void) const { return (m_fHasOnOrderChangedEvent ? true : false); }
		inline bool HasOnOrdersCompletedEvent (void) const { return (m_fHasOnOrdersCompletedEvent ? true : false); }
		inline bool HasShipName (void) const { return !m_sShipNames.IsBlank(); }
		void InstallEquipment (CShip *pShip);
		inline bool IsDebugOnly (void) { return (m_pPlayerSettings && m_pPlayerSettings->IsDebugOnly()); }
		inline bool IsPlayerShip (void) { return (m_pPlayerSettings != NULL); }
		inline bool IsShownAtNewGame (void) { return (m_pPlayerSettings && m_pPlayerSettings->IsInitialClass()); }
		inline bool IsTimeStopImmune (void) { return (m_fTimeStopImmune ? true : false); }
		void LoadImages (void);
		void MarkImages (void);
		void Paint (CG16bitImage &Dest, 
					int x, 
					int y, 
					const ViewportTransform &Trans, 
					int iDirection, 
					int iTick, 
					bool bThrusting,
					bool bRadioactive);
		void PaintMap (CG16bitImage &Dest, 
					int x, 
					int y, 
					const ViewportTransform &Trans, 
					int iDirection, 
					int iTick, 
					bool bThrusting,
					bool bRadioactive);

		//	CDesignType overrides
		static CShipClass *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipClass) ? (CShipClass *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual CCommunicationsHandler *GetCommsHandler (void);
		virtual int GetLevel (void) const { return m_iLevel; }
		virtual DesignTypes GetType (void) { return designShipClass; }
		virtual bool IsVirtual (void) const { return (m_fVirtual ? true : false); }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx);
		virtual void OnReinit (void);
		virtual void OnUnbindDesign (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum PrivateConstants
			{
			maxExhaustImages = 5
			};

		struct SEquipmentDesc
			{
			Abilities iEquipment;
			bool bInstall;
			};

		struct SExhaustDesc
			{
			C3DConversion PosCalc;
			};

		int ComputeDeviceLevel (const SDeviceDesc &Device);
		static int ComputeLevel (int iScore);
		void ComputeMovementStats (CDeviceDescList &Devices, int *retiSpeed, int *retiThrust, int *retiManeuver);
		int ComputeScore (const CDeviceDescList &Devices,
						  int iArmorLevel,
						  int iPrimaryWeapon,
						  int iSpeed,
						  int iThrust,
						  int iManeuver,
						  bool bPrimaryIsLauncher);
		void CreateWreckImage (void);
		void FindBestMissile (IItemGenerator *pItems, int *ioLevel);
		void FindBestMissile (const CItemList &Items, int *ioLevel);
		CString GetGenericName (DWORD *retdwFlags = NULL);
		CPlayerSettings *GetPlayerSettingsInherited (void) const;
		CStationType *GetWreckDesc (void);
		void InitShipNamesIndices (void);
		void PaintThrust (CG16bitImage &Dest, 
						int x, 
						int y, 
						const ViewportTransform &Trans, 
						int iDirection, 
						int iTick,
						bool bInFrontOnly);

		CString m_sManufacturer;				//	Name of manufacturer
		CString m_sName;						//	Class name
		CString m_sTypeName;					//	Name of type
		DWORD m_dwClassNameFlags;				//	Flags for class name

		CString m_sShipNames;					//	Names to use for individual ship
		DWORD m_dwShipNameFlags;				//	Flags for ship name
		CIntArray m_ShipNamesIndices;			//	Shuffled indices for ship names
		int m_iShipName;						//	Current ship name index

		int m_iScore;							//	Score when destroyed
		int m_iLevel;							//	Ship class level

		int m_iMass;							//	Empty mass (tons)
		int m_iCargoSpace;						//	Available cargo space (tons)
		int m_iRotationRange;					//	Number of rotation positions
		int m_iRotationAngle;					//	Angler per rotation
		int m_iManeuverability;					//	Ticks per turn angle
		int m_iManeuverDelay;					//	Ticks per turn angle
		DriveDesc m_DriveDesc;					//	Drive descriptor
		ReactorDesc m_ReactorDesc;				//	Reactor descriptor
		int m_iCyberDefenseLevel;				//	Cyber defense level

		int m_iMaxArmorMass;					//	Max mass of single armor segment
		int m_iMaxCargoSpace;					//	Max amount of cargo space with expansion (tons)
		int m_iMaxReactorPower;					//	Max compatible reactor power
		int m_iMaxDevices;						//	Max number of devices
		int m_iMaxWeapons;						//	Max number of weapon devices (including launchers)
		int m_iMaxNonWeapons;					//	Max number of non-weapon devices

		int m_iLeavesWreck;						//	Chance that it leaves a wreck
		int m_iStructuralHP;					//	Structual hp of wreck
		CStationTypeRef m_pWreckType;				//	Station type to use as wreck

		CStructArray m_Hull;					//	Array of HullSections
		IDeviceGenerator *m_pDevices;			//	Generator of devices
		CDeviceDescList m_AverageDevices;		//	Average complement of devices (only for stats)

		TArray<SEquipmentDesc> m_Equipment;		//	Initial equipment

		CAISettings m_AISettings;				//	AI controller data
		CPlayerSettings *m_pPlayerSettings;		//	Player settings data
		IItemGenerator *m_pItems;				//	Random items

		//	Escorts
		IShipGenerator *m_pEscorts;				//	Escorts

		//	Docking
		int m_iDockingPortsCount;				//	Number of docking ports
		CVector *m_DockingPorts;			//	Position of docking ports
		CDockScreenTypeRef m_pDefaultScreen;	//	Default screen
		DWORD m_dwDefaultBkgnd;					//	Default background screen

		CCommunicationsHandler m_OriginalCommsHandler;
		CCommunicationsHandler m_CommsHandler;	//	Communications handler

		//	Image
		CObjectImageArray m_Image;				//	Image of ship

		//	Wreck image
		CG16bitImage m_WreckBitmap;				//	Image to use when ship is wrecked
		CObjectImageArray m_WreckImage;			//	Image to use when ship is wrecked

		//	Explosion
		CWeaponFireDescRef m_pExplosionType;	//	Explosion to create when ship is destroyed

		//	Exhaust
		CObjectImageArray m_ExhaustImage;		//	Image of drive exhaust
		TArray<SExhaustDesc> m_Exhaust;			//	Drive exhaust painting

		//	Misc
		DWORD m_fRadioactiveWreck:1;			//	TRUE if wreck is always radioactive
		DWORD m_fHasDockingPorts:1;				//	TRUE if ship has docking ports
		DWORD m_fTimeStopImmune:1;				//	TRUE if ship is immune to stop-time
		DWORD m_fHasOnOrderChangedEvent:1;		//	TRUE if ship has an event on each new order
		DWORD m_fHasOnAttackedByPlayerEvent:1;	//	TRUE if ship has an event when attacked by player
		DWORD m_fHasOnObjDockedEvent:1;			//	TRUE if ship has an event when other ships dock
		DWORD m_fHasOnOrdersCompletedEvent:1;	//	TRUE if ship has an event when orders complete
		DWORD m_fCommsHandlerInit:1;			//	TRUE if comms handler has been initialized
		DWORD m_fVirtual:1;						//	TRUE if ship class is virtual (e.g., a base class)
		DWORD m_fInheritedPlayerSettings:1;		//	TRUE if m_pPlayerSettings is inherited from a base class
		DWORD m_fSpare:21;
	};

//	CEffectCreator ------------------------------------------------------------

class CEffectCreator : public CDesignType
	{
	public:
		CEffectCreator (void) : m_iSound(-1), m_pDamage(NULL) { }
		virtual ~CEffectCreator (void);

		static ALERROR CreateBeamEffect (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateTypeFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CEffectCreator **retpCreator);
		static ALERROR CreateFromTag (const CString &sTag, CEffectCreator **retpCreator);
		static IEffectPainter *CreatePainterFromStream (SLoadCtx &Ctx, bool bNullCreator = false);
		static IEffectPainter *CreatePainterFromStreamAndCreator (SLoadCtx &Ctx, CEffectCreator *pCreator);
		static CEffectCreator *FindEffectCreator (const CString &sUNID);
		static void WritePainterToStream (IWriteStream *pStream, IEffectPainter *pPainter);

		inline CWeaponFireDesc *GetDamageDesc (void) { return m_pDamage; }
		inline int GetSound (void) { return m_iSound; }
		inline const CString &GetUNIDString (void) { return m_sUNID; }
		bool IsValidUNID (void);
		void PlaySound (CSpaceObject *pSource = NULL);

		//	Virtuals

		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0);
		virtual IEffectPainter *CreatePainter (void) = 0;
		virtual int GetLifetime (void) { return 0; }
		virtual CEffectCreator *GetSubEffect (int iIndex) { return NULL; }
		virtual CString GetTag (void) = 0;
		virtual void LoadImages (void) { }
		virtual void MarkImages (void) { }
		virtual void SetLifetime (int iLifetime) { }
		virtual void SetVariants (int iVariants) { }

		//	CDesignType overrides
		static CEffectCreator *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEffectType) ? (CEffectCreator *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designEffectType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID);

		//	CEffectCreator overrides

		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) { return NOERROR; }
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }

	private:
		CString m_sUNID;

		DWORD m_dwSoundUNID;
		int m_iSound;

		CWeaponFireDesc *m_pDamage;
	};

//	CEnergyFieldType ----------------------------------------------------------

class CEnergyFieldType : public CDesignType
	{
	public:
		CEnergyFieldType (void);
		virtual ~CEnergyFieldType (void);

		bool AbsorbsWeaponFire (CInstalledDevice *pWeapon);
		int GetDamageAbsorbed (CSpaceObject *pSource, SDamageCtx &Ctx);
		inline CEffectCreator *GetEffectCreator (void) const { return m_pEffect; }
		inline CEffectCreator *GetHitEffectCreator (void) const { return m_pHitEffect; }
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
		inline bool HasOnUpdateEvent (void) { return m_bHasOnUpdateEvent; }
		inline bool IsHitEffectAlt (void) { return m_bAltHitEffect; }
		inline bool RotatesWithShip (void) { return m_bRotateWithShip; }

		//	CDesignType overrides
		static CEnergyFieldType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEnergyFieldType) ? (CEnergyFieldType *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designEnergyFieldType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID);

	private:
		int m_iAbsorbAdj[damageCount];			//	Damage absorbed by the field
		DamageTypeSet m_WeaponSuppress;			//	Types of weapons suppressed
		int m_iBonusAdj[damageCount];			//	Adjustment to weapons damage

		CEffectCreator *m_pEffect;				//	Effect for field
		CEffectCreator *m_pHitEffect;			//	Effect when field is hit by damage

		bool m_bHasOnUpdateEvent;				//	TRUE if we have OnUpdate
		bool m_bAltHitEffect;					//	If TRUE, hit effect replaces normal effect
		bool m_bRotateWithShip;					//	If TRUE, we rotate along with source rotation
	};

//	CSystemType ---------------------------------------------------------------

class CSystemType : public CDesignType
	{
	public:
		enum ECachedHandlers
			{
			evtOnObjJumpPosAdj			= 0,

			evtCount					= 1,
			};

		CSystemType (void);
		virtual ~CSystemType (void);

		inline bool FindEventHandlerSystemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		ALERROR FireOnCreate (SSystemCreateCtx &SysCreateCtx, CString *retsError = NULL);
		bool FireOnObjJumpPosAdj (CSpaceObject *pPos, CVector *iovPos);
		ALERROR FireSystemCreateCode (SSystemCreateCtx &SysCreateCtx, ICCItem *pCode, CString *retsError);
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		CXMLElement *GetLocalSystemTables (void);

		//	CDesignType overrides
		static CSystemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemType) ? (CSystemType *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designSystemType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CXMLElement *m_pDesc;
		SEventHandlerDesc m_CachedEvents[evtCount];
	};

//	CDockScreenType -----------------------------------------------------------

class CDockScreenType : public CDesignType
	{
	public:
		CDockScreenType (void);
		virtual ~CDockScreenType (void);

		static CString GetStringUNID (CDesignType *pRoot, const CString &sScreen);
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		static CDesignType *ResolveScreen (CDesignType *pLocalScreen, const CString &sScreen, CString *retsScreenActual = NULL, bool *retbIsLocal = NULL);

		//	CDesignType overrides
		static CDockScreenType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designDockScreen) ? (CDockScreenType *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designDockScreen; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CXMLElement *m_pDesc;
	};

//	CStationType --------------------------------------------------------------

class CStationType : public CDesignType
	{
	public:
		CStationType (void);
		virtual ~CStationType (void);
		static void Reinit (void);

		inline bool AlertWhenAttacked (void) { return (mathRandom(1, 100) <= m_iAlertWhenAttacked); }
		inline bool AlertWhenDestroyed (void) { return (mathRandom(1, 100) <= m_iAlertWhenDestroyed); }
		inline bool CanAttack (void) const { return (m_fCanAttack ? true : false); }
		inline bool CanBeEncountered (void) { return (!IsUnique() || !m_fEncountered); }
		bool CanBeEncountered (CSystem *pSystem);
		inline bool CanBeHitByFriends (void) { return (m_fNoFriendlyTarget ? false : true); }
		inline bool CanHitFriends (void) { return (m_fNoFriendlyFire ? false : true); }
		CString GenerateRandomName (const CString &sSubst, DWORD *retdwFlags);
		inline CXMLElement *GetAbandonedScreen (void) { return m_pAbandonedDockScreen.GetDesc(); }
		inline CDesignType *GetAbandonedScreen (CString *retsName) { return m_pAbandonedDockScreen.GetDockScreen(this, retsName); }
		inline CArmorClass *GetArmorClass (void) { return (m_pArmor ? m_pArmor->GetArmorClass() : NULL); }
		inline CEffectCreator *GetBarrierEffect (void) { return m_pBarrierEffect; }
		inline IShipGenerator *GetConstructionTable (void) { return m_pConstruction; }
		CSovereign *GetControllingSovereign (void);
		inline DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		inline CString GetDestNodeID (void) { return m_sStargateDestNode; }
		inline CString GetDestEntryPoint (void) { return m_sStargateDestEntryPoint; }
		inline int GetEjectaAdj (void) { return m_iEjectaAdj; }
		CWeaponFireDesc *GetEjectaType (void) { return m_pEjectaType; }
		inline Metric GetEnemyExclusionRadius (void) const { return m_rEnemyExclusionRadius; }
		CWeaponFireDesc *GetExplosionType (void) { return m_pExplosionType; }
		inline int GetEncounterFrequency (void) { return m_iEncounterFrequency; }
		inline IShipGenerator *GetEncountersTable (void) { return m_pEncounters; }
		inline int GetFireRateAdj (void) { return m_iFireRateAdj; }
		inline CXMLElement *GetFirstDockScreen (void) { return m_pFirstDockScreen.GetDesc(); }
		inline CDesignType *GetFirstDockScreen (CString *retsName) { return m_pFirstDockScreen.GetDockScreen(this, retsName); }
		int GetFrequencyByLevel (int iLevel);
		int GetFrequencyForSystem (CSystem *pSystem);
		inline CEffectCreator *GetGateEffect (void) { return m_pGateEffect; }
		inline const CObjectImageArray &GetImage (const CCompositeImageSelector &Selector, DWORD dwModifiers, int *retiRotation = NULL) { return m_Image.GetImage(Selector, dwModifiers, retiRotation); }
		inline IImageEntry *GetImageRoot (void) const { return m_Image.GetRoot(); }
		inline int GetImageVariants (void) { return m_iImageVariants; }
		inline int GetInitialHitPoints (void) { return m_iHitPoints; }
		inline IShipGenerator *GetInitialShips (void) { return m_pInitialShips; }
		inline const CString &GetLocationCriteria (void) { return m_sLocationCriteria; }
		inline Metric GetMass (void) { return m_rMass; }
		inline int GetMinShips (void) { return m_iMinShips; }
		inline Metric GetMaxEffectiveRange (void) { return m_rMaxAttackDistance; }
		inline int GetMaxHitPoints (void) { return m_iMaxHitPoints; }
		inline int GetMaxShipConstruction (void) { return m_iMaxConstruction; }
		inline int GetMaxStructuralHitPoints (void) { return m_iMaxStructuralHP; }
		const CString &GetName (DWORD *retdwFlags = NULL);
		inline DWORD GetNameFlags (void) { return m_dwNameFlags; }
		CString GetNounPhrase (DWORD dwFlags);
		inline Metric GetParallaxDist (void) const { return m_rParallaxDist; }
		inline IItemGenerator *GetRandomItemTable (void) { return m_pItems; }
		inline DWORD GetRandomNameFlags (void) { return m_dwRandomNameFlags; }
		inline IShipGenerator *GetReinforcementsTable (void) { return m_pReinforcements; }
		inline int GetRepairRate (void) { return m_iRepairRate; }
		inline CXMLElement *GetSatellitesDesc (void) { return m_pSatellitesDesc; }
		inline ScaleTypes GetScale (void) const { return m_iScale; }
		inline int GetShipConstructionRate (void) { return m_iShipConstructionRate; }
		inline int GetShipRepairRate (void) { return m_iShipRepairRate; }
		inline CSovereign *GetSovereign (void) const { return m_pSovereign; }
		inline COLORREF GetSpaceColor (void) { return m_rgbSpaceColor; }
		inline int GetStealth (void) const { return m_iStealth; }
		inline int GetStructuralHitPoints (void) { return m_iStructuralHP; }
		inline CTradingDesc *GetTradingDesc (void) { return m_pTrade; }
		inline bool HasAnimations (void) const { return (m_pAnimations != NULL); }
		inline bool HasOnObjDockedEvent (void) const { return (m_fHasOnObjDockedEvent ? true : false); }
		inline bool HasRandomNames (void) const { return !m_sRandomNames.IsBlank(); }
		inline bool HasWreckImage (void) const { return (!IsImmutable() && m_iMaxHitPoints > 0); }
		inline bool IsActive (void) { return (m_fInactive ? false : true); }
		inline bool IsBackgroundObject (void) { return (m_fBackground ? true : false); }
		inline bool IsBeacon (void) { return (m_fBeacon ? true : false); }
		inline bool IsBlacklistEnabled (void) { return (m_fNoBlacklist ? false : true); }
		inline bool IsDestroyWhenEmpty (void) { return (m_fDestroyWhenEmpty ? true : false); }
		inline bool IsEnemyDockingAllowed (void) { return (m_fAllowEnemyDocking ? true : false); }
		inline bool IsImmutable (void) const { return (m_fImmutable ? true : false); }
		inline bool IsMultiHull (void) { return (m_fMultiHull ? true : false); }
		inline bool IsMobile (void) const { return (m_fMobile ? true : false); }
		inline bool IsRadioactive (void) { return (m_fRadioactive ? true : false); }
		inline bool IsSign (void) { return (m_fSign ? true : false); }
		inline bool IsShipEncounter (void) { return (m_fShipEncounter ? true : false); }
		inline bool IsStatic (void) { return (m_fStatic ? true : false); }
		inline bool IsTimeStopImmune (void) { return (m_fTimeStopImmune ? true : false); }
		inline bool IsUnique (void) { return (m_fUnique ? true : false); }
		inline bool IsUniqueInSystem (void) { return (m_fUniqueInSystem ? true : false); }
		inline bool IsWall (void) { return (m_fWall ? true : false); }
		void LoadImages (const CCompositeImageSelector &Selector);
		void MarkImages (const CCompositeImageSelector &Selector);
		void PaintAnimations (CG16bitImage &Dest, int x, int y, int iTick);
		void SetImageSelector (CStation *pStation, CCompositeImageSelector *retSelector);
		inline void SetEncountered (bool bEncountered = true) { m_fEncountered = bEncountered; }
		inline bool ShowsMapIcon (void) { return (m_fNoMapIcon ? false : true); }
		inline bool UsesReverseArticle (void) { return (m_fReverseArticle ? true : false); }

		//	CDesignType overrides
		static CStationType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designStationType) ? (CStationType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual int GetLevel (void) const;
		virtual DesignTypes GetType (void) { return designStationType; }
		virtual bool IsVirtual (void) const { return (m_fVirtual ? true : false); }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx);
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx);
		virtual void OnReinit (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		struct SAnimationSection
			{
			int m_x;
			int m_y;
			CObjectImageArray m_Image;
			};

		CString ComposeLoadError (const CString &sError);
		void InitStationDamage (void);

		CXMLElement *m_pDesc;

		//	Basic station descriptors
		CString m_sName;								//	Name
		DWORD m_dwNameFlags;							//	Flags
		CString m_sRandomNames;							//	Random names
		DWORD m_dwRandomNameFlags;						//	Flags
		CSovereignRef m_pSovereign;						//	Sovereign
		ScaleTypes m_iScale;							//	Scale
		Metric m_rParallaxDist;							//	Parallax distance for background objects
		int m_iLevel;									//	Station level
		Metric m_rMass;									//	Mass of station
		int m_iFireRateAdj;								//	Fire rate adjustment

		//	Armor & HP
		CItemTypeRef m_pArmor;							//	Armor class
		int m_iHitPoints;								//	Hit points at creation time
		int m_iMaxHitPoints;							//	Max hit points
		int m_iRepairRate;								//	HP repaired every 10 ticks
		int m_iStealth;									//	Stealth

		int m_iStructuralHP;							//	Initial structural hit points
		int m_iMaxStructuralHP;							//	Max structural hp (0 = station is permanent)

		//	Devices
		int m_iDevicesCount;							//	Number of devices in array
		CInstalledDevice *m_Devices;					//	Device array

		//	Items
		IItemGenerator *m_pItems;						//	Random item table
		CTradingDesc *m_pTrade;							//	Trading structure

		DWORD m_fMobile:1;								//	Station moves
		DWORD m_fWall:1;								//	Station is a wall
		DWORD m_fInactive:1;							//	Station starts inactive
		DWORD m_fDestroyWhenEmpty:1;					//	Station is destroyed when last item removed
		DWORD m_fAllowEnemyDocking:1;					//	Station allows enemies to dock
		DWORD m_fNoFriendlyFire:1;						//	Station cannot hit friends
		DWORD m_fSign:1;								//	Station is a text sign
		DWORD m_fBeacon:1;								//	Station is a nav beacon
		DWORD m_fRadioactive:1;							//	Station is radioactive
		DWORD m_fUnique:1;								//	Station is unique
		DWORD m_fEncountered:1;							//	Unique station has already been encountered
		DWORD m_fCanAttack:1;							//	Station is active (i.e., will react if attacked)
		DWORD m_fShipEncounter:1;						//	This is a ship encounter
		DWORD m_fImmutable:1;							//	Station can not take damage or become radioactive, etc.
		DWORD m_fNoMapIcon:1;							//	Do not show on map
		DWORD m_fMultiHull:1;							//	Only harmed by WMD damage
		DWORD m_fTimeStopImmune:1;						//	TRUE if station is immune to time-stop
		DWORD m_fNoBlacklist:1;							//	Does not blacklist player if attacked
		DWORD m_fReverseArticle:1;						//	Use "a" instead of "an" and vice versa
		DWORD m_fHasOnObjDockedEvent:1;					//	Station has OnObjDocked event
		DWORD m_fStatic:1;								//	Use CStatic instead of CStation
		DWORD m_fBackground:1;							//	Background object
		DWORD m_fUniqueInSystem:1;						//	Unique in system
		DWORD m_fNoFriendlyTarget:1;					//	Station cannot be hit by friends
		DWORD m_fVirtual:1;								//	Virtual stations do not show up
		DWORD m_fSpare:7;

		//	Images
		CCompositeImageDesc m_Image;
		int m_iImageVariants;							//	Number of variants
		CIntArray m_ShipWrecks;							//	Class IDs to use as image (for shipwrecks)
		int m_iAnimationsCount;							//	Number of animation sections
		SAnimationSection *m_pAnimations;				//	Animation sections (may be NULL)

		//	Docking
		CDockScreenTypeRef m_pFirstDockScreen;			//	First screen (may be NULL)
		CDockScreenTypeRef m_pAbandonedDockScreen;		//	Screen to use when abandoned (may be NULL)
		DWORD m_dwDefaultBkgnd;							//	Default background screen

		//	Behaviors
		int m_iAlertWhenAttacked;						//	Chance that station will warn others when attacked
		int m_iAlertWhenDestroyed;						//	Chance that station will warn others when destroyed
		Metric m_rMaxAttackDistance;					//	Max range at which station guns attack

		//	Random population
		CString m_sLevelFrequency;						//	String array of frequency distribution by level
		CString m_sLocationCriteria;					//	Criteria for location
		Metric m_rEnemyExclusionRadius;					//	No enemy stations within this radius

		//	Ships
		IShipGenerator *m_pInitialShips;				//	Ships at creation time
		IShipGenerator *m_pReinforcements;				//	Reinforcements table
		int m_iMinShips;								//	Min ships at station
		IShipGenerator *m_pEncounters;					//	Random encounters table
		int m_iEncounterFrequency;						//	Frequency of random encounter
		int m_iShipRepairRate;							//	HP repaired every 10 ticks (per docked ship)
		IShipGenerator *m_pConstruction;				//	Ships built by station
		int m_iShipConstructionRate;					//	Ticks between each construction
		int m_iMaxConstruction;							//	Stop building when we get this many ships

		//	Satellites
		CXMLElement *m_pSatellitesDesc;

		//	Explosion
		CWeaponFireDescRef m_pExplosionType;			//	Explosion to create when station is destroyed

		//	Ejecta
		int m_iEjectaAdj;								//	Adjustment to probability for ejecta when hit by weapon
														//		0 = no chance of ejecta
														//		100 = normal chance
														//		>100 = greater than normal chance
		CWeaponFireDescRef m_pEjectaType;				//	Type of ejecta generated

		//	Miscellaneous
		COLORREF m_rgbSpaceColor;						//	Space color
		CEffectCreatorRef m_pBarrierEffect;				//	Effect when object hits station
		CSovereignRef m_pControllingSovereign;			//	If controlled by different sovereign
														//	(e.g., centauri occupation)
		CString m_sStargateDestNode;					//	Dest node
		CString m_sStargateDestEntryPoint;				//	Dest entry point
		CEffectCreatorRef m_pGateEffect;				//	Effect when object gates in/out of station
	};

//	CEconomyType --------------------------------------------------------------

class CEconomyType : public CDesignType
	{
	public:
		CEconomyType (void) { }

		CurrencyValue Exchange (CEconomyType *pFrom, CurrencyValue iAmount);
		inline const CString &GetCurrencyNamePlural (void) { return m_sCurrencyPlural; }
		inline const CString &GetCurrencyNameSingular (void) { return m_sCurrencySingular; }
		inline const CString &GetSID (void) { return m_sSID; }
		inline bool IsCreditEquivalent (void) { return (m_iCreditConversion == 100); }
		static CString RinHackGet (CSpaceObject *pObj);
		static CurrencyValue RinHackInc (CSpaceObject *pObj, CurrencyValue iInc);
		static void RinHackSet (CSpaceObject *pObj, const CString &sData);

		//	CDesignType overrides
		static CEconomyType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEconomyType) ? (CEconomyType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual DesignTypes GetType (void) { return designEconomyType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CString m_sSID;									//	String ID (e.g., "credit")
		CString m_sCurrencyName;						//	Annotated name
		CString m_sCurrencySingular;					//	Singular form: "1 credit"
		CString m_sCurrencyPlural;						//	Plural form: "10 credits"; "You don't have enough credits"

		CurrencyValue m_iCreditConversion;				//	Commonwealth Credits that 100 units of the currency is worth
	};

//	CSovereign ----------------------------------------------------------------

enum DispositionClasses
	{
	alignNone =					-1,

	alignConstructiveChaos =	0,
	alignConstructiveOrder =	1,
	alignNeutral =				2,
	alignDestructiveOrder =		3,
	alignDestructiveChaos =		4,
	};

class CSovereign : public CDesignType
	{
	public:
		enum Alignments
			{
			alignUnknown =				-1,

			alignRepublic =				0,	//	constructive	community		knowledge		evolution		(constructive chaos)
			alignFederation =			1,	//	constructive	community		knowledge		tradition		(constructive order)
			alignUplifter =				2,	//	constructive	community		spirituality	evolution		(constructive chaos)
			alignFoundation =			3,	//	constructive	community		spirituality	tradition		(neutral)
			alignCompetitor =			4,	//	constructive	independence	knowledge		evolution		(neutral)
			alignArchivist =			5,	//	constructive	independence	knowledge		tradition		(constructive order)
			alignSeeker =				6,	//	constructive	independence	spirituality	evolution		(constructive chaos)
			alignHermit =				7,	//	constructive	independence	spirituality	tradition		(constructive order)

			alignCollective =			8,	//	destructive		community		knowledge		evolution		(destructive chaos)
			alignEmpire =				9,	//	destructive		community		knowledge		tradition		(destructive order)
			alignSterelizer =			10,	//	destructive		community		spirituality	evolution		(destructive chaos)
			alignCorrector =			11,	//	destructive		community		spirituality	tradition		(destructive order)
			alignMegalomaniac =			12,	//	destructive		independence	knowledge		evolution		(destructive chaos)
			alignCryptologue =			13,	//	destructive		independence	knowledge		tradition		(destructive order)
			alignPerversion =			14,	//	destructive		independence	spirituality	evolution		(destructive chaos)
			alignSolipsist =			15,	//	destructive		independence	spirituality	tradition		(destructive order)

			alignUnorganized =			16,	//	unorganized group of beings										(neutral)
			alignSubsapient =			17,	//	animals, zoanthropes, cyberorgs, and other creatures			(neutral)

			alignCount =				18,
			};

		enum Disposition
			{
			dispEnemy = 0,
			dispNeutral = 1,
			dispFriend = 2,
			};

		CSovereign (void);
		~CSovereign (void);

		void DeleteRelationships (void);
		inline void FlushEnemyObjectCache (void) { m_pEnemyObjectsSystem = NULL; }
		Disposition GetDispositionTowards (CSovereign *pSovereign);
		inline const CSpaceObjectList &GetEnemyObjectList (CSystem *pSystem) { InitEnemyObjectList(pSystem); return m_EnemyObjects; }
		CString GetText (MessageTypes iMsg);
		inline bool IsEnemy (CSovereign *pSovereign) { return (m_bSelfRel || (pSovereign != this)) && (GetDispositionTowards(pSovereign) == dispEnemy); }
		inline bool IsFriend (CSovereign *pSovereign) { return (!m_bSelfRel && (pSovereign == this)) || (GetDispositionTowards(pSovereign) == dispFriend); }
		static Alignments ParseAlignment (const CString &sAlign);
		void SetDispositionTowards (CSovereign *pSovereign, Disposition iDisp);

		//	CDesignType overrides
		static CSovereign *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSovereign) ? (CSovereign *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual DesignTypes GetType (void) { return designSovereign; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx);
		virtual void OnPrepareReinit (void);
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx);
		virtual void OnReinit (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		struct SRelationship
			{
			CSovereign *pSovereign;
			Disposition iDisp;

			SRelationship *pNext;
			};

		bool CalcSelfRel (void);
		SRelationship *FindRelationship (CSovereign *pSovereign);
		inline Alignments GetAlignment (void) { return m_iAlignment; }
		void InitEnemyObjectList (CSystem *pSystem);
		void InitRelationships (void);

		CString m_sName;
		Alignments m_iAlignment;
		CXMLElement *m_pInitialRelationships;

		SRelationship *m_pFirstRelationship;	//	List of individual relationships

		bool m_bSelfRel;						//	TRUE if relationship with itself is not friendly
		CSystem *m_pEnemyObjectsSystem;			//	System that we've cached enemy objects
		CSpaceObjectList m_EnemyObjects;		//	List of enemy objects that can attack
	};

//	CPower --------------------------------------------------------------------

class CPower : public CDesignType
	{
	public:
		CPower (void);
		virtual ~CPower (void);

		inline ICCItem *GetCode (void) { return m_pCode; }
		inline int GetInvokeCost (void) { return m_iInvokeCost; }
		inline const CString &GetInvokeKey (void) { return m_sInvokeKey; }
		inline const CString &GetName (void) { return m_sName; }
		inline ICCItem *GetOnInvokedByPlayer (void) { return m_pOnInvokedByPlayer; }
		inline ICCItem *GetOnShow (void) { return m_pOnShow; }
		void Invoke (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		void InvokeByPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnShow (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker);

		//	CDesignType overrides
		static CPower *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designPower) ? (CPower *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designPower; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CString m_sName;
		int m_iInvokeCost;
		CString m_sInvokeKey;

		ICCItem *m_pCode;
		ICCItem *m_pOnShow;
		ICCItem *m_pOnInvokedByPlayer;
		ICCItem *m_pOnDestroyCheck;
	};

//	CSpaceEnvironmentType -----------------------------------------------------

class CSpaceEnvironmentType : public CDesignType
	{
	public:
		CSpaceEnvironmentType (void) { }

		ALERROR FireOnUpdate (CSpaceObject *pObj, CString *retsError = NULL);
		inline Metric GetDragFactor (void) { return m_rDragFactor; }
		inline bool HasOnUpdateEvent (void) { return m_bHasOnUpdateEvent; }
		inline bool IsLRSJammer (void) { return m_bLRSJammer; }
		inline bool IsShieldJammer (void) { return m_bShieldJammer; }
		inline bool IsSRSJammer (void) { return m_bSRSJammer; }
		void Paint (CG16bitImage &Dest, int x, int y);
		void PaintLRS (CG16bitImage &Dest, int x, int y);

		//	CDesignType overrides
		static CSpaceEnvironmentType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSpaceEnvironmentType) ? (CSpaceEnvironmentType *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designSpaceEnvironmentType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		CObjectImageArray m_Image;

		bool m_bLRSJammer;				//	If TRUE, LRS is disabled
		bool m_bShieldJammer;			//	If TRUE, shields are disabled
		bool m_bSRSJammer;				//	If TRUE, SRS is disabled
		bool m_bHasOnUpdateEvent;		//	If TRUE, we have an OnUpdate event
		Metric m_rDragFactor;			//	Coefficient of drag (1.0 = no drag)

		//	We check for damage every 15 ticks
		int m_iDamageChance;			//	Chance of damage
		DamageDesc m_Damage;			//	Damage caused to objects in environment
	};

//	CShipTable ----------------------------------------------------------------

class CShipTable : public CDesignType
	{
	public:
		CShipTable (void);
		virtual ~CShipTable (void);

		inline void CreateShips (SShipCreateCtx &Ctx) { if (m_pGenerator) m_pGenerator->CreateShips(Ctx); }
		ALERROR ValidateForRandomEncounter (void) { if (m_pGenerator) return m_pGenerator->ValidateForRandomEncounter(); }

		//	CDesignType overrides
		static CShipTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipTable) ? (CShipTable *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designShipTable; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		IShipGenerator *m_pGenerator;
	};

//	Adventures and Extensions -------------------------------------------------

class CAdventureDesc : public CDesignType
	{
	public:
		void CreateIcon (int cxWidth, int cyHeight, CG16bitImage **retpIcon);
		void FireOnGameEnd (const CGameRecord &Game, const SBasicGameStats &BasicStats);
		void FireOnGameStart (void);
		inline DWORD GetBackgroundUNID (void) { return m_dwBackgroundUNID; }
		inline const CString &GetDesc (void) { return m_sDesc; }
		inline DWORD GetExtensionUNID (void) { return m_dwExtensionUNID; }
		inline const CString &GetName (void) { return m_sName; }
		inline const CString &GetStartingNodeID (void) { return m_sStartingNodeID; }
		inline const CString &GetStartingPos (void) { return m_sStartingPos; }
		ALERROR GetStartingShipClasses (TSortMap<CString, CShipClass *> *retClasses, CString *retsError);
		inline const CString &GetWelcomeMessage (void) { return m_sWelcomeMessage; }
		inline bool IsCurrentAdventure (void) { return (m_fIsCurrentAdventure ? true : false); }
		inline bool IsInDefaultResource (void) { return (m_fInDefaultResource ? true : false); }
		inline bool IsValidStartingClass (CShipClass *pClass) { return pClass->MatchesCriteria(m_StartingShips); }
		inline void SetCurrentAdventure (bool bCurrent = true) { m_fIsCurrentAdventure = bCurrent; }

		//	CDesignType overrides
		static CAdventureDesc *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designAdventureDesc) ? (CAdventureDesc *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue);
		virtual DesignTypes GetType (void) { return designAdventureDesc; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual void OnUnbindDesign (void) { m_fIsCurrentAdventure = false; }

	private:
		DWORD m_dwExtensionUNID;

		CString m_sName;						//	Name of adventure
		DWORD m_dwBackgroundUNID;				//	Background image to use for choice screen
		CString m_sDesc;						//	Description of adventure
		CString m_sWelcomeMessage;				//	Equivalent of "Welcome to Transcendence!"

		CDesignTypeCriteria m_StartingShips;	//	Starting ship criteria
		CString m_sStartingNodeID;				//	NodeID where we start
		CString m_sStartingPos;					//	Named object at which we start

		DWORD m_fIsCurrentAdventure:1;			//	TRUE if this is the current adventure
		DWORD m_fInDefaultResource:1;			//	TRUE if adventure is a module in the default resource
	};

//	Name Generator -------------------------------------------------------------

class CNameGenerator : public CDesignType
	{
	public:
		CNameGenerator (void);
		virtual ~CNameGenerator (void);

		//	CDesignType overrides
		static CNameGenerator *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designNameGenerator) ? (CNameGenerator *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designNameGenerator; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
	};

//	System Map -----------------------------------------------------------------

class ITopologyProcessor
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc);

		virtual ~ITopologyProcessor (void) { }
		inline ALERROR BindDesign (SDesignLoadCtx &Ctx) { return OnBindDesign(Ctx); }
		inline CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		inline ALERROR Process (CSystemMap *pMap, CTopology &Topology, CTopologyNodeList &NodeList, CString *retsError) { return OnProcess(pMap, Topology, NodeList, retsError); }

	protected:
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual ALERROR OnInitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) { return NOERROR; }
		virtual ALERROR OnProcess (CSystemMap *pMap, CTopology &Topology, CTopologyNodeList &NodeList, CString *retsError) { return NOERROR; }

		CTopologyNodeList *FilterNodes (CTopology &Topology, CTopologyNode::SCriteria &Criteria, CTopologyNodeList &Unfiltered, CTopologyNodeList &Filtered);
		void RestoreMarks (CTopology &Topology, TArray<bool> &Saved);
		void SaveAndMarkNodes (CTopology &Topology, CTopologyNodeList &NodeList, TArray<bool> *retSaved);
	};

class CSystemMap : public CDesignType
	{
	public:
		struct SNodeExtra
			{
			CTopologyNode *pNode;
			int iStargateCount;
			};

		struct SSortEntry
			{
			SNodeExtra *pExtra;
			int iSort;
			};

		CSystemMap (void) : m_bAdded(false), m_bDebugShowAttributes(false) { }
		virtual ~CSystemMap (void);

		void AddAnnotation (CEffectCreator *pEffect, int x, int y, int iRotation, DWORD *retdwID = NULL);
		ALERROR AddFixedTopology (CTopology &Topology, CString *retsError);
		bool DebugShowAttributes (void) const { return m_bDebugShowAttributes; }
		CG16bitImage *CreateBackgroundImage (void);
		void GetBackgroundImageSize (int *retcx, int *retcy);
		inline CSystemMap *GetDisplayMap (void) { return (m_pPrimaryMap != NULL ? m_pPrimaryMap : this); }
		inline const CString &GetName (void) const { return m_sName; }
		inline void GetScale (int *retiInitial, int *retiMin, int *retiMax) { if (retiInitial) *retiInitial = m_iInitialScale; if (retiMin) *retiMin = m_iMinScale; if (retiMax) *retiMax = m_iMaxScale; }

		//	CDesignType overrides
		static CSystemMap *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemMap) ? (CSystemMap *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designSystemMap; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx);
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID);
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx);
		virtual void OnReinit (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		struct SMapAnnotation
			{
			DWORD dwID;

			IEffectPainter *pPainter;
			int xOffset;
			int yOffset;
			int iTick;
			int iRotation;
			};

		CString m_sName;						//	Name of the map (for the player)
		DWORD m_dwBackgroundImage;				//	Background image to use
		int m_iInitialScale;					//	Initial map display scale (100 = 100%)
		int m_iMaxScale;						//	Max zoom
		int m_iMinScale;						//	Min zoom

		CSystemMapRef m_pPrimaryMap;			//	If not NULL, place nodes on given map
		TArray<CSystemMapRef> m_Uses;			//	List of maps that we rely on.

		//	Topology generation
		CTopologyDescTable m_FixedTopology;
		TArray<ITopologyProcessor *> m_Processors;

		//	Annotations
		TArray<SMapAnnotation> m_Annotations;

		//	Temporaries
		bool m_bAdded;							//	TRUE if map was added to topology

		//	Debug
		bool m_bDebugShowAttributes;
	};

int KeyCompare (const CSystemMap::SSortEntry &Key1, const CSystemMap::SSortEntry &Key2);

//	System Tables --------------------------------------------------------------

class CSystemTable : public CDesignType
	{
	public:
		CSystemTable (void) : m_pTable(NULL) { }
		virtual ~CSystemTable (void);

		CXMLElement *FindElement (const CString &sElement) const { return (m_pTable ? m_pTable->GetContentElementByTag(sElement) : NULL); }

		//	CDesignType overrides
		static CSystemTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemTable) ? (CSystemTable *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designSystemTable; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { m_pTable = pDesc->OrphanCopy(); return NOERROR; }

	private:
		CXMLElement *m_pTable;
	};

//	Template Types -------------------------------------------------------------

class CTemplateType : public CDesignType
	{
	public:
		CTemplateType (void) { }
		virtual ~CTemplateType (void) { }

		//	CDesignType overrides
		static CTemplateType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designTemplateType) ? (CTemplateType *)pType : NULL); }
		virtual DesignTypes GetType (void) { return designTemplateType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }

	private:
	};

//	Associated Structures ------------------------------------------------------

class CInstalledDevice
	{
	public:
		CInstalledDevice (void);

		inline bool AbsorbDamage (CSpaceObject *pShip, SDamageCtx &Ctx) { if (!IsEmpty()) return m_pClass->AbsorbDamage(this, pShip, Ctx); else return false; }
		inline bool Activate (CSpaceObject *pSource, 
							  CSpaceObject *pTarget,
							  bool *retbSourceDestroyed,
							  bool *retbConsumedItems = NULL)
			{ return m_pClass->Activate(this, pSource, pTarget, retbSourceDestroyed, retbConsumedItems); }
		int CalcPowerUsed (CSpaceObject *pSource);
		inline bool CanBeDamaged (void) { return m_pClass->CanBeDamaged(); }
		inline bool CanBeDisabled (void) { return m_pClass->CanBeDisabled(); }
		inline bool CanHitFriends (void) { return m_pClass->CanHitFriends(); }
		inline bool CanRotate (CItemCtx &Ctx) { return m_pClass->CanRotate(Ctx); }
		inline void Deplete (CSpaceObject *pSource) { m_pClass->Deplete(this, pSource); }
		void FinishInstall (CSpaceObject *pSource);
		int GetActivateDelay (CSpaceObject *pSource);
		inline int GetActivateDelayAdj (void) { return m_iActivateDelayAdj; }
		inline int GetBonus (void) const { return m_iBonus; }
		inline ItemCategories GetCategory (void) { return m_pClass->GetCategory(); }
		inline int GetCharges (CSpaceObject *pSource) { return m_iCharges; }
		inline CDeviceClass *GetClass (void) const { return m_pClass; }
		inline int GetCounter (CSpaceObject *pSource, CDeviceClass::CounterTypes *retiCounter = NULL) { return m_pClass->GetCounter(this, pSource, retiCounter); }
		inline const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return m_pClass->GetDamageDesc(Ctx); }
		inline int GetDamageType (int iVariant = -1) { return m_pClass->GetDamageType(this, iVariant); }
		inline DWORD GetData (void) const { return m_dwData; }
		inline int GetDefaultFireAngle (CSpaceObject *pSource) { return m_pClass->GetDefaultFireAngle(this, pSource); }
		bool GetDeviceEnhancementDesc (CSpaceObject *pSource, CInstalledDevice *pWeapon, SDeviceEnhancementDesc *retDesc) { return m_pClass->GetDeviceEnhancementDesc(this, pSource, pWeapon, retDesc); }
		inline int GetDeviceSlot (void) const { return m_iDeviceSlot; }
		inline DWORD GetDisruptedDuration (void) const { return (m_pItem ? m_pItem->GetDisruptedDuration() : 0); }
		inline const DriveDesc *GetDriveDesc (CSpaceObject *pSource) { return m_pClass->GetDriveDesc(this, pSource); }
		CString GetEnhancedDesc (CSpaceObject *pSource, const CItem *pItem = NULL);
		inline const ReactorDesc *GetReactorDesc (CSpaceObject *pSource) { return m_pClass->GetReactorDesc(this, pSource); }
		inline int GetFireArc (void) const { return (IsOmniDirectional() ? 360 : AngleRange(m_iMinFireArc, m_iMaxFireArc)); }
		inline int GetFireAngle (void) const { return m_iFireAngle; }
		inline CItem *GetItem (void) const { return m_pItem; }
		inline DWORD GetLinkedFireOptions (CItemCtx &Ctx) { return m_pClass->GetLinkedFireOptions(Ctx); }
		inline int GetMinFireArc (void) const { return m_iMinFireArc; }
		inline Metric GetMaxEffectiveRange (CSpaceObject *pSource, CSpaceObject *pTarget = NULL) { return m_pClass->GetMaxEffectiveRange(pSource, this, pTarget); }
		inline int GetMaxFireArc (void) const { return m_iMaxFireArc; }
		inline const CItemEnhancement &GetMods (void) const { return (m_pItem ? m_pItem->GetMods() : CItem::GetNullMod()); }
		inline CString GetName (void) { return m_pClass->GetName(); }
		inline CEnergyField *GetOverlay (void) const { return m_pOverlay; }
		CVector GetPos (CSpaceObject *pSource);
		inline int GetPosAngle (void) const { return m_iPosAngle; }
		inline int GetPosRadius (void) const { return m_iPosRadius; }
		inline int GetPosZ (void) const { return m_iPosZ; }
		inline int GetPowerRating (CItemCtx &Ctx) { return m_pClass->GetPowerRating(Ctx); }
		inline int GetRotation (void) const { return AngleMiddle(m_iMinFireArc, m_iMaxFireArc); }
		inline void GetSelectedVariantInfo (CSpaceObject *pSource, 
											CString *retsLabel,
											int *retiAmmoLeft,
											CItemType **retpType = NULL)
			{ m_pClass->GetSelectedVariantInfo(pSource, this, retsLabel, retiAmmoLeft, retpType); }
		inline void GetStatus (CShip *pShip, int *retiStatus, int *retiMaxStatus) { m_pClass->GetStatus(this, pShip, retiStatus, retiMaxStatus); }
		inline CSpaceObject *GetTarget (CSpaceObject *pSource) const;
		inline int GetTemperature (void) const { return m_iTemperature; }
		inline int GetTimeUntilReady (void) const { return m_iTimeUntilReady; }
		inline int GetValidVariantCount (CSpaceObject *pSource) { return m_pClass->GetValidVariantCount(pSource, this); }
		inline int GetWeaponEffectiveness (CSpaceObject *pSource, CSpaceObject *pTarget) { return m_pClass->GetWeaponEffectiveness(pSource, this, pTarget); }
		int IncCharges (CSpaceObject *pSource, int iChange);
		inline void IncTemperature (int iChange) { m_iTemperature += iChange; }
		void InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitFromDesc (const SDeviceDesc &Desc);
		void Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iDeviceSlot, bool bInCreate = false);
		inline bool IsAutomatedWeapon (void) { return m_pClass->IsAutomatedWeapon(); }
		inline bool IsAreaWeapon (CSpaceObject *pSource) { return m_pClass->IsAreaWeapon(pSource, this); }
		inline bool IsDamaged (void) const { return (m_pItem ? m_pItem->IsDamaged() : false); }
		inline bool IsDirectional (void) const { return (m_iMinFireArc != m_iMaxFireArc); }
		inline bool IsDisrupted (void) const { return (m_pItem ? m_pItem->IsDisrupted() : false); }
		inline bool IsEmpty (void) const { return m_pClass == NULL; }
		inline bool IsEnabled (void) const { return m_fEnabled; }
		inline bool IsEnhanced (void) const { return (m_pItem ? m_pItem->IsEnhanced() : false); }
		inline bool IsExternal (void) const { return m_fExternal; }
		inline bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return m_pClass->IsFuelCompatible(Ctx, FuelItem); }
		inline bool IsLastActivateSuccessful (void) const { return m_fLastActivateSuccessful; }
		bool IsLinkedFire (CItemCtx &Ctx, ItemCategories iTriggerCat) const;
		inline bool IsOmniDirectional (void) const { return (m_fOmniDirectional ? true : false); }
		inline bool IsOptimized (void) const { return m_fOptimized; }
		inline bool IsOverdrive (void) const { return m_fOverdrive; }
		inline bool IsReady (void) const { return (m_iTimeUntilReady == 0); }
		inline bool IsRegenerating (void) const { return (m_fRegenerating ? true : false); }
		inline bool IsSecondaryWeapon (void) const { return ((m_fSecondaryWeapon ? true : false) || (m_pClass->GetLinkedFireOptions(CItemCtx()) != 0)); }
		inline bool IsTriggered (void) const { return (m_fTriggered ? true : false); }
		inline bool IsVariantSelected (CSpaceObject *pSource) { return (m_pClass ? m_pClass->IsVariantSelected(pSource, this) : true); }
		inline bool IsWaiting (void) const { return (m_fWaiting ? true : false); }
		inline bool IsWeaponAligned (CSpaceObject *pShip, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) { return m_pClass->IsWeaponAligned(pShip, this, pTarget, retiAimAngle, retiFireAngle); }
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void ReadFromStream (CSpaceObject *pSource, SLoadCtx &Ctx);
		inline void Recharge (CShip *pShip, int iStatus) { m_pClass->Recharge(this, pShip, iStatus); }
		inline void Reset (CSpaceObject *pShip) { m_pClass->Reset(this, pShip); }
		inline void SelectFirstVariant (CSpaceObject *pSource) { m_pClass->SelectFirstVariant(pSource, this); }
		inline void SelectNextVariant (CSpaceObject *pSource, int iDir = 1) { m_pClass->SelectNextVariant(pSource, this, iDir); }
		inline void SetActivateDelayAdj (int iAdj) { m_iActivateDelayAdj = iAdj; }
		inline void SetBonus (int iBonus) { m_iBonus = iBonus; }
		inline void SetClass (CDeviceClass *pClass) { m_pClass.Set(pClass); }
		inline void SetChargesCache (int iCharges) { m_iCharges = iCharges; }
		inline void SetData (DWORD dwData) { m_dwData = dwData; }
		inline void SetDeviceSlot (int iDev) { m_iDeviceSlot = iDev; }
		inline void SetEnabled (bool bEnabled) { m_fEnabled = bEnabled; }
		inline void SetFireAngle (int iAngle) { m_iFireAngle = iAngle; }
		inline void SetFireArc (int iMinFireArc, int iMaxFireArc) { m_iMinFireArc = iMinFireArc; m_iMaxFireArc = iMaxFireArc; }
		inline void SetLastActivateSuccessful (bool bSuccessful) { m_fLastActivateSuccessful = bSuccessful; }
		inline void SetOmniDirectional (bool bOmnidirectional = true) { m_fOmniDirectional = bOmnidirectional; }
		inline void SetOptimized (bool bOptimized) { m_fOptimized = bOptimized; }
		inline void SetOverdrive (bool bOverdrive) { m_fOverdrive = bOverdrive; }
		inline void SetOverlay (CEnergyField *pOverlay) { m_pOverlay = pOverlay; }
		inline void SetPosAngle (int iAngle) { m_iPosAngle = iAngle; }
		inline void SetPosRadius (int iRadius) { m_iPosRadius = iRadius; }
		inline void SetPosZ (int iZ) { m_iPosZ = iZ; m_f3DPosition = (iZ != 0); }
		inline void SetRegenerating (bool bRegenerating) { m_fRegenerating = bRegenerating; }
		inline void SetTemperature (int iTemperature) { m_iTemperature = iTemperature; }
		inline void SetTarget (CSpaceObject *pObj);
		inline void SetTimeUntilReady (int iDelay) { m_iTimeUntilReady = iDelay; }
		inline void SetTriggered (bool bTriggered) { m_fTriggered = bTriggered; }
		inline void SetWaiting (bool bWaiting) { m_fWaiting = bWaiting; }
		inline bool ShowActivationDelayCounter (CSpaceObject *pSource) { return m_pClass->ShowActivationDelayCounter(pSource, this); }
		void Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList);
		void Update (CSpaceObject *pSource, 
					 int iTick, 
					 bool *retbSourceDestroyed,
					 bool *retbConsumedItems = NULL,
					 bool *retbDisrupted = NULL);
		void WriteToStream (IWriteStream *pStream);

	private:
		CItem *m_pItem;							//	Item installed in this slot
		CDeviceClassRef m_pClass;				//	The device class that is installed here
		CEnergyField *m_pOverlay;				//	Overlay (if associated)
		DWORD m_dwTargetID;						//	ObjID of target (for tracking secondary weapons)

		DWORD m_dwData;							//	Data specific to device class

		int m_iDeviceSlot:16;					//	Device slot

		int m_iPosAngle:16;						//	Position of installation (degrees)
		int m_iPosRadius:16;					//	Position of installation (pixels)
		int m_iPosZ:16;							//	Position of installation (height)
		int m_iMinFireArc:16;					//	Min angle of fire arc (degrees)
		int m_iMaxFireArc:16;					//	Max angle of fire arc (degrees)

		int m_iTimeUntilReady:16;				//	Timer counting down until ready to activate
		int m_iFireAngle:16;					//	Last fire angle

		int m_iBonus:16;						//	Bonus for weapons (+1, etc.)
		int m_iTemperature:16;					//	Temperature for weapons
		int m_iCharges:16;						//	Charges
		int m_iActivateDelayAdj:16;				//	Adjustment to activation delay

		DWORD m_fOmniDirectional:1;				//	Installed on turret
		DWORD m_fOverdrive:1;					//	Device has overdrive installed
		DWORD m_fOptimized:1;					//	Device is optimized by alien engineers
		DWORD m_fSecondaryWeapon:1;				//	Secondary weapon
		DWORD m_fEnabled:1;						//	Device is enabled
		DWORD m_fExternal:1;					//	Device is external to hull
		DWORD m_fWaiting:1;						//	Waiting for cooldown, etc.
		DWORD m_fTriggered:1;					//	Device trigger is down (e.g., weapon is firing)
		DWORD m_fRegenerating:1;				//	TRUE if we regenerated on the last tick
		DWORD m_fLastActivateSuccessful:1;		//	TRUE if we successfully fired (last time we tried)
		DWORD m_f3DPosition:1;					//	If TRUE we use m_iPosZ to compute position
		DWORD m_dwSpare:20;						//	Spare flags
	};

//	CDesignCollection

class CDesignList
	{
	public:
		CDesignList (void) : m_List(128) { }
		~CDesignList (void) { }

		inline void AddEntry (CDesignType *pType) { m_List.Insert(pType); }
		void Delete (DWORD dwUNID);
		inline void DeleteAll (void) { m_List.DeleteAll(); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CDesignType *GetEntry (int iIndex) const { return m_List[iIndex]; }

	private:
		TArray<CDesignType *> m_List;
	};

class CDesignTable
	{
	public:
		CDesignTable (bool bFreeTypes = false) : m_bFreeTypes(bFreeTypes) { }
		~CDesignTable (void) { DeleteAll(); }

		ALERROR AddEntry (CDesignType *pEntry);
		ALERROR AddOrReplaceEntry (CDesignType *pEntry, CDesignType **retpOldEntry = NULL);
		void Delete (DWORD dwUNID);
		void DeleteAll (void);
		CDesignType *FindByUNID (DWORD dwUNID) const;
		inline int GetCount (void) const { return m_Table.GetCount(); }
		inline CDesignType *GetEntry (int iIndex) const { return m_Table.GetValue(iIndex); }

	private:
		TSortMap<DWORD, CDesignType *> m_Table;
		bool m_bFreeTypes;
	};

enum EExtensionTypes
	{
	extExtension,
	extAdventure,
	};

struct SExtensionDesc
	{
	SExtensionDesc (void) :
			pEntities(NULL),
			Table(true)	//	Owns design types
		{ }

	~SExtensionDesc (void)
		{
		if (pEntities)
			delete pEntities;
		}

	inline CExternalEntityTable *GetEntities (void) const { return pEntities; }
	inline void SetEntities (CExternalEntityTable *pNewEntities) { if (pEntities) delete pEntities; pEntities = pNewEntities; }

	CString sResDb;							//	Resource file for the extension
	EExtensionTypes iType;					//	Either adventure or extension
	DWORD dwUNID;							//	UNID of extension
	CExternalEntityTable *pEntities;		//	Table of XML entities defined by extension.
	bool bDefaultResource;					//	If TRUE, this extension is part of the default resource

	CString sName;							//	name of extension
	DWORD dwVersion;						//	version
	TArray<DWORD> Extends;					//	UNIDs that this extension extends
	TArray<CString> Credits;				//	List of names for credits

	CDesignTable Table;						//	Design types for this extension
	CTopologyDescTable Topology;			//	Topology
	TArray<CString> Modules;				//	Modules

	bool bRegistered;						//	If TRUE, this is a registered extension
	bool bLoaded;							//	If TRUE, we're loaded
	bool bEnabled;							//	If TRUE, we're enabled
	bool bDebugOnly;						//	If TRUE, extension enabled only in debug mode
	};

class CDynamicDesignTable
	{
	public:
		CDynamicDesignTable (void) { }
		~CDynamicDesignTable (void) { CleanUp(); }

		ALERROR DefineType (SExtensionDesc *pExtension, DWORD dwUNID, const CString &sSource, CDesignType **retpType = NULL, CString *retsError = NULL);
		void Delete (DWORD dwUNID);
		inline void DeleteAll (void) { CleanUp(); }
		inline int GetCount (void) const { return m_Table.GetCount(); }
		inline CDesignType *GetType (int iIndex) const { return m_Table[iIndex].pType; }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SEntry
			{
			SExtensionDesc *pExtension;
			DWORD dwUNID;
			CString sSource;
			CDesignType *pType;
			};

		void CleanUp (void);
		ALERROR Compile (SEntry *pEntry, CDesignType **retpType, CString *retsError = NULL);
		inline SEntry *GetEntry (int iIndex) const { return &m_Table[iIndex]; }

		TSortMap<DWORD, SEntry> m_Table;
	};

struct SDesignLoadCtx
	{
	SDesignLoadCtx (void) :
			pResDb(NULL),
			pExtension(NULL),
			bNewGame(false),
			bNoResources(false),
			bNoVersionCheck(false),
			bLoadAdventureDesc(false),
			bLoadModule(false)
		{ }

	//	Context
	CString sResDb;							//	ResourceDb filespec
	CResourceDb *pResDb;					//	Open ResourceDb object
	CString sFolder;						//	Folder context (used when loading images)
	SExtensionDesc *pExtension;				//	Extesion desc
	bool bLoadAdventureDesc;				//	If TRUE, we are loading an adventure desc only
	bool bLoadModule;						//	If TRUE, we are loading elements in a module

	//	Options
	bool bNewGame;							//	If TRUE, then we are binding a new game
	bool bNoResources;
	bool bNoVersionCheck;

	//	Output
	CString sError;
	};

class CDesignCollection
	{
	public:
		enum ECachedHandlers
			{
			evtGetGlobalAchievements	= 0,
			evtGetGlobalDockScreen		= 1,
			evtOnGlobalDockPaneInit		= 2,
			evtOnGlobalObjDestroyed		= 3,
			evtOnGlobalUniverseCreated	= 4,
			evtOnGlobalUniverseLoad		= 5,
			evtOnGlobalUniverseSave		= 6,

			evtCount					= 7
			};

		CDesignCollection (void);
		~CDesignCollection (void);

		ALERROR AddDynamicType (SExtensionDesc *pExtension, DWORD dwUNID, const CString &sSource, bool bNewGame, CString *retsError);
		ALERROR AddSystemTable (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR BeginLoadAdventure (SDesignLoadCtx &Ctx, SExtensionDesc *pExtension);
		ALERROR BeginLoadAdventureDesc (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bDefaultResources, CExternalEntityTable *pEntities);
		ALERROR BeginLoadExtension (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CExternalEntityTable *pEntities);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		void CleanUp (void);
		void ClearImageMarks (void);
		void EndLoadAdventure (SDesignLoadCtx &Ctx) { Ctx.pExtension = NULL; }
		void EndLoadAdventureDesc (SDesignLoadCtx &Ctx) { Ctx.pExtension = NULL; Ctx.bLoadAdventureDesc = false; }
		void EndLoadExtension (SDesignLoadCtx &Ctx) { Ctx.pExtension = NULL; }
		CAdventureDesc *FindAdventureDesc (DWORD dwUNID) const;
		CAdventureDesc *FindAdventureForExtension (DWORD dwUNID) const;
		inline CEconomyType *FindEconomyType (const CString &sID) { CEconomyType **ppType = m_EconomyIndex.GetAt(sID); return (ppType ? *ppType : NULL); }
		inline CDesignType *FindEntry (DWORD dwUNID) const { return m_AllTypes.FindByUNID(dwUNID); }
		SExtensionDesc *FindExtension (DWORD dwUNID) const;
		CXMLElement *FindSystemFragment (const CString &sName, CSystemTable **retpTable = NULL) const;
		void FireGetGlobalAchievements (CGameStats &Stats);
		bool FireGetGlobalDockScreen (CSpaceObject *pObj, CString *retsScreen, int *retiPriority = NULL);
		void FireOnGlobalObjDestroyed (SDestroyCtx &Ctx);
		void FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane);
		void FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip);
		void FireOnGlobalPlayerEnteredSystem (void);
		void FireOnGlobalPlayerLeftSystem (void);
		void FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx);
		ALERROR FireOnGlobalTypesInit (SDesignLoadCtx &Ctx);
		void FireOnGlobalUniverseCreated (void);
		void FireOnGlobalUniverseLoad (void);
		void FireOnGlobalUniverseSave (void);
		inline CExternalEntityTable &GetBaseEntities (void) { return m_BaseEntities; }
		inline int GetCount (void) const { return m_AllTypes.GetCount(); }
		inline int GetCount (DesignTypes iType) const { return m_ByType[iType].GetCount(); }
		DWORD GetDynamicUNID (const CString &sName);
		void GetEnabledExtensions (TArray<DWORD> *retExtensionList);
		inline CDesignType *GetEntry (int iIndex) const { return m_AllTypes.GetEntry(iIndex); }
		inline CDesignType *GetEntry (DesignTypes iType, int iIndex) const { return m_ByType[iType].GetEntry(iIndex); }
		inline SExtensionDesc *GetExtension (int iIndex) { return m_Extensions[iIndex]; }
		inline int GetExtensionCount (void) { return m_Extensions.GetCount(); }
		CG16bitImage *GetImage (DWORD dwUNID, bool bCopy = false);
		CTopologyDescTable *GetTopologyDesc (void) const { return m_pTopology; }
		inline bool HasDynamicTypes (void) { return (m_DynamicTypes.GetCount() > 0); }
		bool IsAdventureExtensionBound (DWORD dwUNID);
		bool IsAdventureExtensionLoaded (DWORD dwUNID);
		bool IsRegisteredGame (void);
		ALERROR LoadAdventureDescMainRes (SDesignLoadCtx &Ctx, CAdventureDesc *pAdventure);
		ALERROR LoadEntryFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CDesignType **retpType = NULL);
		void ReadDynamicTypes (SUniverseLoadCtx &Ctx);
		void Reinit (void);
		void SelectAdventure (CAdventureDesc *pAdventure);
		ALERROR SelectExtensions (CAdventureDesc *pAdventure, TArray<DWORD> *pExtensionList, bool *retbBindNeeded, CString *retsError);
		void SweepImages (void);
		void WriteDynamicTypes (IWriteStream *pStream);

	private:
		ALERROR AddEntry (SDesignLoadCtx &Ctx, CDesignType *pEntry);
		ALERROR AddExtension (SDesignLoadCtx &Ctx, EExtensionTypes iType, DWORD dwUNID, bool bDefaultResource, SExtensionDesc **retpExtension);
		void CacheGlobalEvents (CDesignType *pType);
		ALERROR CreateTemplateTypes (SDesignLoadCtx &Ctx);
		bool IsExtensionCompatibleWithAdventure (SExtensionDesc *pExtension, CAdventureDesc *pAdventure);
		ALERROR LoadDesignType (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadExtensionDesc (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bDefaultResource, SExtensionDesc **retpExtension);

		//	Loaded types. These are initialized at load-time and never change.
		CDesignTable m_Base;
		CTopologyDescTable m_BaseTopology;
		CExternalEntityTable m_BaseEntities;
		TSortMap<DWORD, SExtensionDesc *> m_Extensions;

		//	Cached data initialized at bind-time
		CDesignTable m_AllTypes;
		CDesignList m_ByType[designCount];
		CTopologyDescTable *m_pTopology;
		SExtensionDesc *m_pAdventureExtension;
		CAdventureDesc *m_pAdventureDesc;
		TSortMap<CString, CEconomyType *> m_EconomyIndex;
		CGlobalEventCache *m_EventsCache[evtCount];

		//	Dynamic design types

		CDynamicDesignTable m_DynamicTypes;
		TSortMap<CString, CDesignType *> m_DynamicUNIDs;
	};

//	Utility functions

IShipController *CreateShipController (const CString &sAI);
DWORD ExtensionVersionToInteger (DWORD dwVersion);
CString GenerateRandomName (const CString &sList, const CString &sSubst);
CString GenerateRandomNameFromTemplate (const CString &sName, const CString &sSubst = NULL_STR);
CString GetDamageName (DamageTypes iType);
CString GetDamageShortName (DamageTypes iType);
int GetDiceCountFromAttribute(const CString &sValue);
int GetFrequency (const CString &sValue);
int GetFrequencyByLevel (const CString &sLevelFrequency, int iLevel);
CString GetItemCategoryName (ItemCategories iCategory);
bool IsConstantName (const CString &sList);
bool IsEnergyDamage (DamageTypes iType);
bool IsMatterDamage (DamageTypes iType);
COLORREF LoadCOLORREF (const CString &sString);
ALERROR LoadDamageAdj (CXMLElement *pItem, const CString &sAttrib, int *retiAdj, int *retiCount = NULL);
ALERROR LoadDamageAdj (CXMLElement *pDesc, int *pDefAdj, int *retiAdj);
DamageTypes LoadDamageTypeFromXML (const CString &sAttrib);
DWORD LoadExtensionVersion (const CString &sVersion);
DWORD LoadNameFlags (CXMLElement *pDesc);
WORD LoadRGBColor (const CString &sString);
DWORD LoadUNID (SDesignLoadCtx &Ctx, const CString &sString);

//	Inline implementations

inline EDamageResults CInstalledArmor::AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx) { return m_pArmorClass->AbsorbDamage(CItemCtx(pSource, this), Ctx); }
inline int CInstalledArmor::GetMaxHP (CSpaceObject *pSource) { return m_pArmorClass->GetMaxHP(CItemCtx(pSource, this)); }

inline CEconomyType *CItem::GetCurrencyType (void) const { return m_pItemType->GetCurrencyType(); }
inline const CString &CItem::GetDesc (void) const { return m_pItemType->GetDesc(); }

inline CDeviceClass *CDeviceDescList::GetDeviceClass (int iIndex) const { return m_pDesc[iIndex].Item.GetType()->GetDeviceClass(); }

inline Metric CArmorClass::GetMass (void) const { return m_pItemType->GetMass(); }
inline CString CArmorClass::GetName (void) { return m_pItemType->GetNounPhrase(); }
inline DWORD CArmorClass::GetUNID (void) { return m_pItemType->GetUNID(); }

inline int CDeviceClass::GetLevel (void) const { return m_pItemType->GetLevel(); }
inline Metric CDeviceClass::GetMass (void) { return m_pItemType->GetMass(); }
inline CString CDeviceClass::GetName (void) { return m_pItemType->GetName(NULL); }
inline DWORD CDeviceClass::GetUNID (void) { return m_pItemType->GetUNID(); }

inline CXMLElement *CItemType::GetUseScreen (void) const { return m_pUseScreen.GetDesc(); }
inline CDesignType *CItemType::GetUseScreen (CString *retsName) { return m_pUseScreen.GetDockScreen(this, retsName); }

inline bool DamageDesc::IsEnergyDamage (void) const { return ::IsEnergyDamage(m_iType); }
inline bool DamageDesc::IsMatterDamage (void) const { return ::IsEnergyDamage(m_iType); }

inline void IEffectPainter::PlaySound (CSpaceObject *pSource) { GetCreator()->PlaySound(pSource); }

inline CSystemMap *CTopologyNode::GetDisplayPos (int *retxPos, int *retyPos) { if (retxPos) *retxPos = m_xPos; if (retyPos) *retyPos = m_yPos; return (m_pMap ? m_pMap->GetDisplayMap() : NULL); }

#endif