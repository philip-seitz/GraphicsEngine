class Renderer
{
public:
	// static class (keine Instanz)

	static void Init();
	static void Shutdown();


	static void BeginBatch();
	static void EndBatch();
	static void Flush();


	static void DrawTriangle();
	static void Clear();

private:
	// stats o.ä. zum Debuggen

};