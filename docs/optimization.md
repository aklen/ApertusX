📄 ApertusX Memory Usage Analysis & Optimization

🧠 Current Observation

The ApertusX runtime memory usage sits around 14 MB (RSS) upon startup. This is relatively normal for a modular C++ framework but may be reduced with careful analysis and tuning.

⸻

🔍 Dynamically Linked Libraries (via otool -L)

./apertus:
  @rpath/libapertus_myplugin.dylib
  @rpath/libapertus_plugin_gstreamer.dylib
  @rpath/libapertus_core.dylib
  @rpath/libfruit.dylib
  /opt/homebrew/opt/gstreamer/lib/libgstreamer-1.0.0.dylib
  /opt/homebrew/opt/glib/lib/libgobject-2.0.0.dylib
  /opt/homebrew/opt/glib/lib/libglib-2.0.0.dylib
  /opt/homebrew/opt/gettext/lib/libintl.8.dylib
  /usr/lib/libcurl.4.dylib
  /usr/lib/libc++.1.dylib
  /usr/lib/libSystem.B.dylib

📦 Library Impact Summary

Library	Purpose	Est. Memory Impact
libapertus_core.dylib	Core system: events, DI, logging	🟢 2–4 MB
libfruit.dylib	Google Fruit dependency injection	🟡 ~1–2 MB
libgstreamer-1.0.0.dylib	Full GStreamer runtime	🔴 3–8 MB
libglib-2.0.0.dylib	GStreamer base dependencies	🟡 ~1–2 MB
libgobject-2.0.0.dylib	GObject system for GStreamer	🟡 ~1–2 MB
libintl.8.dylib	Gettext localization	⚪ Negligible
libcurl.4.dylib	HTTP/REST client support	⚪ ~0.5–1 MB



⸻

🔧 Optimization Recommendations

1. Lazy GStreamer Plugin Initialization
	•	Avoid registering the GStreamer plugin at startup.
	•	Load it only when needed using event triggers.

if (eventService->IsAudioRequested()) {
    pluginService->RegisterPlugin(gstreamerPlugin);
    gstreamerPlugin->Init(); // Deferred init
}



⸻

2. Reduce Thread Stack Size

By default, each thread may allocate 8 MB of stack. If your threads don’t need it, reduce:

pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_attr_setstacksize(&attr, 1024 * 1024); // 1MB
pthread_create(&thread, &attr, threadFunction, nullptr);
pthread_attr_destroy(&attr);



⸻

3. Rebuild GStreamer Without NLS (gettext)

If localization is not needed, rebuild GStreamer with:

./configure --disable-nls

This removes libintl.dylib as a dependency.

⸻

4. Use Lazy Injection (Google Fruit)

Use Provider<T> or other techniques to avoid constructing all services upfront.

fruit::Provider<IMediaService> mediaProvider;



⸻

5. Reduce Live555 RTP Buffers

Live555 uses large RTP buffers. Reduce them:

OutPacketBuffer::maxSize = 200000; // Instead of default 500000+



⸻

6. Strip Binary (Disk Size Only)

To reduce the binary size (not RAM), you can strip debug symbols:

strip ./apertus



⸻

🧪 Tools for Memory Analysis (macOS)

Tool	Purpose
Instruments (Xcode)	Memory leaks, allocations, profiling
ps, top	Live monitoring from terminal
leaks <PID>	Apple CLI memory leak checker
malloc_history	Detailed per-malloc call tracing

Note: valgrind is not supported on macOS ARM64.

⸻

🧠 Library Comparison Table (For Realtime Sync / Streaming)

Feature	Live555	SnapCast	RakNet	SLikeNet	ENet
Protocol	UDP (RTP)	TCP + UDP	UDP/TCP	UDP/TCP	UDP
Low-latency	✅ Yes	❌ No	✅ Yes	✅ Yes	✅ Yes
Multi-room audio sync	⚠️ Manual	✅ Built-in	❌	❌	❌
Left-Right channel sync	⚠️ Complex	✅ Native	❌	❌	❌
Precise time sync (PTP/NTP)	❌ No	✅ Yes	❌	❌	❌
Multicast	✅ Yes	❌	⚠️ Optional	⚠️ Optional	❌
Custom data streaming	✅ ByteArray	❌ Audio only	✅ Yes	✅ Yes	✅ Yes
Scripting API	❌ Low-level	✅ JSON-RPC	❌ C++ only	❌ C++ only	❌ C only
Ease of integration	⚠️ Moderate	✅ Easy	⚠️ Complex	⚠️ Complex	✅ Simple



⸻

✅ Conclusion
	•	14 MB baseline usage is normal for an application that uses DI, plugins, Live555, and GStreamer.
	•	For memory-sensitive environments, lazy-loading and buffer tuning are your best optimization paths.
	•	Use Live555 or JRTPLIB for custom byte-stream replication.
	•	Use SnapCast only if precise audio playback sync (e.g. multi-room) is required.

⸻
