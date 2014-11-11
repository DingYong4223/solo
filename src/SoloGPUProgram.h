#ifndef __SOLO_GPU_PROGRAM_H__
#define __SOLO_GPU_PROGRAM_H__

#include "SoloIGPUProgram.h"
#include "SoloCommonInternal.h"

namespace solo
{
	class GPUProgram: public IGPUProgram
	{
	public:
		GPUProgram(const str &vsShaderSrc, const str &fsShaderSrc);
		virtual ~GPUProgram();

		virtual bool valid() const override
		{
			return _valid;
		}

		virtual str log() const override
		{
			return _log;
		}

		virtual size_t id() const override
		{
			return _id;
		}

	protected:
		bool _valid;
		str _log;
		size_t _id;

		void _appendToLog(const str &newLog);
	};
}

#endif