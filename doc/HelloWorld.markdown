# HelloWorld

----------
##�����
1������Pebble
���ȣ�����ȵ���������Pebble
2�����밲װ
������ƪ�ĵ������Pebble�ı��롢��װ

##����

###ͨ��IDL����ǰ��˽����ӿ�
	namespace cpp tutorial

	service Tutorial {
	   string helloworld(1: string who)
	}

###���ɴ���
####C++�����
	pebble -gen cpp helloworld.pebble
####�ͻ��ˣ�Unity��
	pebble -gen csharp helloworld.pebble

###C++�����
####����ʵ��
	class TutorialHandler : public TutorialCobSvIf {
	public:
		void helloworld(const std::string& who, cxx::function<void(std::string const& _return)> cob) {
			std::cout << who << " say helloworld!" << std::endl;
			cob("Hello " + who + "!");
		}
	};
####��������
	// ��ʼ��PebbleServer
	pebble::PebbleServer pebble_server;
	pebble_server.Init(argc, argv, "cfg/pebble.ini");
	// ע�����
	TutorialHandler service;
	pebble_server.RegisterService(&service);
	// ��ӷ��������ַ
	pebble_server.AddServiceManner("http://0.0.0.0:8300", pebble::rpc::PROTOCOL_BINARY);
	// ����server
	pebble_server.Start();


###Unity�ͻ���
####��ʼ��
	if (!HttpTransportCreator.Initial())
	{
		throw new Exception("Init HTTPTransportCreator fail!");
	}

	Rpc.Instance.SetDefaultCommunicationParam("http://10.12.234.103:8300");//�������API����Ĭ�ϵ�ͨѶ����
	client = Rpc.Instance.GetClient<Tutorial.Client>();//����ͨѶ����������ʱ��SetDefaultCommunicationParam�����õ�ֵ

####ÿ֡��Tick
	Rpc.Instance.Update();

####RPC����
	client.helloworld("John", (ex, result) =>
	{
		if (ex != null)
		{
			Debug.Log("RPC BaseService.heartbeat exception: " + ex);
		}
		else
		{
			Debug.Log("RPC BaseService.heartbeat return: " + result);
		}
	});






