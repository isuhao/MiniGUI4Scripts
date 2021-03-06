#if defined (_MGNCSCTRL_SCROLLBAR) || defined (_MGNCSCTRL_TRACKBAR)

#ifndef _MGUI_NCSCTRL_SLIDERPIECE_H
#define _MGUI_NCSCTRL_SLIDERPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSliderPieceClass mSliderPieceClass;
typedef struct _mSliderPiece mSliderPiece;

#define mSliderPieceClassHeader(clss, superCls) \
	mContainerPieceClassHeader(clss, superCls) \
	int (*stepLine)(clss*, BOOL forward); \
	int (*stepPage)(clss*, BOOL forward);

struct _mSliderPieceClass
{
	mSliderPieceClassHeader(mSliderPiece, mContainerPiece)
};

MGNCS_EXPORT extern mSliderPieceClass g_stmSliderPieceCls;

#define mSliderPieceHeader(clss) \
	mContainerPieceHeader(clss)  \
	mHotPiece * thumb; \
	unsigned char flags; \
	unsigned char thumb_state; \
	unsigned short line_step; \
	unsigned short page_step; \
	unsigned short cur_pos; \
	unsigned short min; \
	unsigned short max;

#define NCSS_SLIDERPIECE_VERT   0x01
#define NCSS_SLIDERPIECE_TICK   0x02
#define NCSS_SLIDERPIECE_MIN    0x04
#define NCSS_SLIDERPIECE_MAX    0x08
#define NCSS_SLIDERPIECE_CENTERTHUMB 0x10 //the start point at the center of thumb

#define mSliderPiece_setVert(self)    (((self)->flags)|= NCSS_SLIDERPIECE_VERT)
#define mSliderPiece_setHorz(self)    (((self)->flags)&= (~NCSS_SLIDERPIECE_VERT))
#define mSliderPiece_isVert(self)     ((((self)->flags)&NCSS_SLIDERPIECE_VERT) == NCSS_SLIDERPIECE_VERT)

#define mSliderPiece_setTick(self)    (((self)->flags)|=NCSS_SLIDERPIECE_TICK)
#define mSliderPiece_clearTick(self)  (((self)->flags)&=(~NCSS_SLIDERPIECE_TICK))
#define mSliderPiece_isTick(self)     (((self)->flags)&NCSS_SLIDERPIECE_TICK)

#define mSliderPiece_setMin(self)     (((self)->flags)|=NCSS_SLIDERPIECE_MIN)
#define mSliderPiece_clearMin(self)   (((self)->flags)&=(~NCSS_SLIDERPIECE_MIN))
#define mSliderPiece_isMin(self)      ((self)->flags & NCSS_SLIDERPIECE_MIN)

#define mSliderPiece_setMax(self)     (((self)->flags)|=NCSS_SLIDERPIECE_MAX)
#define mSliderPiece_clearMax(self)   (((self)->flags)&=(~NCSS_SLIDERPIECE_MAX))
#define mSliderPiece_isMax(self)      ((self)->flags & NCSS_SLIDERPIECE_MAX)

#define mSliderPiece_setCenterThumb(self)  (((self)->flags) |= NCSS_SLIDERPIECE_CENTERTHUMB)
#define mSliderPiece_clearCenterThumb(self) (((self)->flags) &= (~NCSS_SLIDERPIECE_CENTERTHUMB))
#define mSliderPiece_isCenterThumb(self)  ((self)->flags & NCSS_SLIDERPIECE_CENTERTHUMB)

struct _mSliderPiece
{
	mSliderPieceHeader(mSliderPiece)
};

enum mSliderPieceProps{
	NCSP_SLIDERPIECE_POS = SLIDERPIECE_PROP_BEGIN,
	NCSP_SLIDERPIECE_LINESTEP ,
	NCSP_SLIDERPIECE_PAGESTEP ,
	NCSP_SLIDERPIECE_MAX,
	NCSP_SLIDERPIECE_MIN,
	NCSP_SLIDERPIECE_TICK,

	NCSP_SLIDERPIECE_DIRECTION = PIECECOMM_PROP_DIRECTION
};

enum mSliderPieceEvent
{
	NCSN_SLIDERPIECE_POSCHANGED = SLIDERPIECE_EVENT_BEGIN,
	NCSN_SLIDERPIECE_REACHMAX,
	NCSN_SLIDERPIECE_REACHMIN
};

#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_SCROLLBAR _MGNCSCTRL_TRACKBAR
