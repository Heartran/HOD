--
-- Lobby
--

-- �^�C�g����ʎ���
subclass("Stage", Actor) {
		__tostring = function(self)
			return("class Title name="..self.name) 
		end
}

-- �R���X�g���N�^
function Stage:ctor(...)
	print("Stage:ctor() called")
	Stage.super.ctor(self, ...)
end

-- ������
function Stage:init(cfg)
	-- �X�[�p�[�N���X(Actor)��init���Ă�
	print("Stage:init");
	Actor.init(self, cfg)
end

-- Actor�폜���Ɏ��s�����
function Stage:on_destroy()
	print("Stage:on_destroy()");
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	state:resetAll();
	GUI.Manager:destory()
	Telop.destroy();
end


-- ��Ԋ֐�

function Stage:state_start(rt)
	print("Stage:state_start");
		
	-- �����̐ݒ�
	local telop = [[
@0, 200
�����ɂ����ς��|���Ă�������
@200, 200
���N���b�N �w��ʒu�Ɉړ�
@400, 200
�E�N���b�N �U��
@600, 200
�A�ł���Ƌ����U�����o���
@900, 200 : 0xffff0000
�킠�I������!
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
	resource:loadCameraAnimation("Data/MMDData/vmd/camera_simple.vmd")
	resource:loadPlayer("Data/XmlLoader/GameObject/Player/miku.xml")
	resource:loadEnemy("Data/XmlLoader/GameObject/Enemy/bou.xml")
	resource:loadStage("Data/XmlLoader/GameObject/Stage/stage01.xml");
	resource:loadEnemy("Data/XmlLoader/GameObject/Enemy/mikudayo.xml")
	--
	local gameDrawer = MME.Drawer.GameDrawer:instance();
	
	local isOk = resource:isReady() and
		gameDrawer:isReady()
	--	
	--
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
	
	
	init_transform:getOrigin():setValue( -250, 55,-2 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -300, 55,-2 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 55,-2 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -400, 55,-2 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:setIdentity()
	init_transform:getBasis():setEulerZYX(
		0,
		-3.14/2.5,
		0 );
	
	init_transform:getOrigin():setValue( -250, 55, 40 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -300, 55, 40 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 55, 40 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:setIdentity()
	init_transform:getBasis():setEulerZYX(
		0,
		-3.14/1.5,
		0 );
	
	init_transform:getOrigin():setValue( -250, 55, -40 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -300, 55, -40 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -350, 55, -40 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getBasis():setEulerZYX(
		0,
		-3.14/2,
		0 );
	
	
	init_transform:getOrigin():setValue( -500, 55,-45 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55,-30 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55,-15 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55, 0 );
	state:reserveEnemySpawn("�_�l��", 0.1, init_transform )
	
	
	
	
	init_transform:getBasis():setEulerZYX(
		0,
		-3.14/2,
		0 );
	
		
	init_transform:getOrigin():setValue( -450, 55, -90 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55, -75 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55, -60 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55,-45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55,-30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55,-15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55,-0 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55, 15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55, 30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -450, 55, 45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	
	init_transform:getOrigin():setValue( -600, 55, -90 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55, -75 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55, -60 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55,-45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55,-30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55,-15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55,-0 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55, 15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55, 30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -600, 55, 45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	
	init_transform:getOrigin():setValue( -550, 55, -90 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55, -75 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55, -60 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55,-45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55,-30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55,-15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55,-0 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55, 15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55, 30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -550, 55, 45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
		init_transform:getOrigin():setValue( -500, 55, -90 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55, -75 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55, -60 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55,-45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55,-30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55,-15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55,-0 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55, 15 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55, 30 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	init_transform:getOrigin():setValue( -500, 55, 45 );
	state:reserveEnemySpawn("�~�N�_���[", 15, init_transform )
	
	
	
	init_transform:delete()
	


	if isOk then
		return "goto", "state_frame"
	else
		change_scene(Title)
		wait();
	end
	
	return "exit";
		
end


function Stage:state_frame(rt)
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
		-- 
		if state:getPlayerHp()<= 0 then
				local cfg={};
				cfg.txt = "Game Over!\n�c�O�ł����I\n";
				cfg.score = state:getScore()-2000;
				cfg.numKilled = state:getNumberKilledEnemy();
				cfg.playerHp = state:getPlayerHp()
				change_scene(Clear, cfg )
				self:wait(0)
		end
		
		self:wait(0)
	end
	
	
	local cfg={};
	cfg.txt = "Game Clear!\n���߂łƂ��������܂��I\n";
	cfg.score = state:getScore();
	cfg.numKilled = state:getNumberKilledEnemy();
	cfg.playerHp = state:getPlayerHp()
	change_scene(Clear, cfg )
	
	
	-- 3�b��G�o��
	--���̓G�͉�ʊO�܂���10�b�o�߂ō폜
	-- if true then
	-- end
	--
	
	return "exit"
end

