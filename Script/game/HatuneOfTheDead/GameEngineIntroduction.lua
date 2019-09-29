--
-- Lobby
--

-- �^�C�g����ʎ���
subclass("GameEngineIntroduction", Actor) {
		__tostring = function(self)
			return("class Title name="..self.name) 
		end
}

-- �R���X�g���N�^
function GameEngineIntroduction:ctor(...)
	print("GameEngineIntroduction:ctor() called")
	GameEngineIntroduction.super.ctor(self, ...)
end

-- ������
function GameEngineIntroduction:init(cfg)
	-- �X�[�p�[�N���X(Actor)��init���Ă�
	print("GameEngineIntroduction:init");
	Actor.init(self, cfg)
end

-- Actor�폜���Ɏ��s�����
function GameEngineIntroduction:on_destroy()
	print("GameEngineIntroduction:on_destroy()");
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance();
	state:resetAll();
	GUI.Manager:destory();
	Telop.destroy();
	
end


-- ��Ԋ֐�

function GameEngineIntroduction:state_start(rt)
	print("GameEngineIntroduction:state_start");
	
		--�����̐ݒ�
-- 	local telop = [[
-- @0, 200
-- �ȒP��GameEngine�̐������������Ǝv���܂�
-- @200, 200
-- ����GameEngine�̓�����
-- @400, 200 : 0xffff0000
-- MMD��MME�ALua��g�ݍ��񂾂��ƂƂł�
-- @600, 200
-- lua���g�����ƂŃr���h�Ȃ��ŕύX�ł��܂�
-- @800, 200
-- �ł͂���Ă݂܂�
-- ]];

-- 	local telop = [[
-- @0, 200
-- �G�̔z�u,����ς��܂���
-- @200, 200
-- ���łɃ��f�����ς��܂���
-- ]];
--
	local telop = [[
@0, 200
��x�ǂݍ��߂΃��[�h���K�v�Ȃ��݌v�ɂ��Ă��܂�
@200, 200
Lua�Ń��\�[�X���폜�����
@400, 200
�ă��[�h����܂�
]];




	local isOk = Telop.parse( telop, #telop);
	Telop.setTime(0.0);
	
	if isOk then
		print("Telop ok!\n")
	else
		print("Telop NG!\n")
		change_scene(Title)
		wait();
	end
	
	
	
	-- ���\�[�X�ƃC���X�^���X�������Q�[���I�u�W�F�N�g��lua�ł͊Ǘ����Ȃ�
	
	
	-- ���\�[�X�̏���
	local resource = Sequence.HatuneOfTheDead.LuaImpl.Resource:instance()
	
	--�X�N���v�g��ǂݍ��ނ��тɍă��[�h��������Ή�2�s���R�����g�A�E�g
	-- resource:release();
	-- resource = Sequence.HatuneOfTheDead.LuaImpl.Resource:instance()
	
	
	resource:loadCameraAnimation("Data/MMDData/vmd/camera_simple.vmd")
	-- resource:loadPlayer("Data/XmlLoader/GameObject/Player/miku.xml")
	resource:loadPlayer("Data/XmlLoader/GameObject/Player/rin.xml")
	-- resource:loadPlayer("Data/XmlLoader/GameObject/Player/miku_lat.xml")
	resource:loadEnemy("Data/XmlLoader/GameObject/Enemy/bou.xml")
	resource:loadEnemy("Data/XmlLoader/GameObject/Enemy/mikudayo.xml")
	resource:loadStage("Data/XmlLoader/GameObject/Stage/stage01.xml");
	--
	local gameDrawer = MME.Drawer.GameDrawer:instance();
	
	isOk = resource:isReady() and gameDrawer:isReady()
	
	
	while not isOk do
    isOk = 
      resource:isReady() and
		  gameDrawer:isReady()
		self:wait(0)
	end
	
	self.time = 0
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	state:build(resource)
	
	--�G�L�����N�^�[�̔z�u
	local init_transform = btTransform:new()
	init_transform:setIdentity()
	init_transform:getBasis():setEulerZYX(
		0,
		-3.14/2,
		0 );
	
	
	-- �c��3��
	-- init_transform:getOrigin():setValue( -250, 55,-2 );
	-- state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	--
	-- init_transform:getOrigin():setValue( -300, 55,-2 );
	-- state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	--
	-- init_transform:getOrigin():setValue( -350, 55,-2 );
	-- state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	--
	
	-- �㉡��6��
	init_transform:getOrigin():setValue( -350, 55,-2 );
	state:reserveEnemySpawn("�~�N�_���[", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 80,-2 );
	state:reserveEnemySpawn("�~�N�_���[", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 100,-2 );
	state:reserveEnemySpawn("�~�N�_���[", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 55,-20 );
	state:reserveEnemySpawn("�~�N�_���[", 0.1, init_transform )
	
			init_transform:getOrigin():setValue( -350, 55,-2 );
	state:reserveEnemySpawn("�~�N�_���[", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 55, 20 );
	state:reserveEnemySpawn("�~�N�_���[", 0.1, init_transform )
	
	
	

	init_transform:delete()
	
	

	return "goto", "state_frame"
		
end


function GameEngineIntroduction:state_frame(rt)
	local gameDrawer = MME.Drawer.GameDrawer:instance();
	local mme = MME.Manager:instance()
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	local player = state:getPlayer()
	local fw = GameLib.Framework:instance();

	--�Œ�t���[�����[�g
	local elapsedTime=1.0/60.0;

	self.time = self.time+elapsedTime
	
	local black = GameLib.Math.Vector4:new_local(0.0)
	mme:setEdgeColor( black );
	
	
	local time = 0.0
	-- while time < 20 do
	while time < 30 do
		
		time = time+elapsedTime
		
		state:update()
		
		state:draw()
		
		Telop.draw( 1 );--//�t���[���P��
		self:wait(0)
	end
	
	
	
	change_scene(GameEngineIntroduction)
	
	
	return "exit"
end

