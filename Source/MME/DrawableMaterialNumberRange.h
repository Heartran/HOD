#pragma once

namespace MME {

class DrawableMaterialNumberRange {
public:
	//"0,3,5"�̂悤�ɁA�J���}��؂�Ŕԍ���񋓂��邱�ƂŁA�����̔ԍ����w��ł���B
	//�܂��A"6-10"�Ȃǂ̂悤�ɁA�ԍ����n�C�t���łȂ����ƂŁA�͈͎w�肪�ł���B
	//"12-"�̂悤�ɁA�͈͂̊J�n�ԍ��݂̂��w�肵���ꍇ�́A����ȍ~�̑S�Ă̔ԍ����ΏۂƂȂ�B
	DrawableMaterialNumberRange( const char *text );
	~DrawableMaterialNumberRange();

	bool canDraw( unsigned int materialIndex ) const;

private:
	class Impl;
	Impl *mImpl;
};
}

