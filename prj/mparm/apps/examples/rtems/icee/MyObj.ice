module MPARM {
	interface MyObj {
		nonmutating void send(string data);
		idempotent void shutdown();
	};
};

