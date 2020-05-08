//*****************************************************************************
//
//	Copyright 2015 Microsoft Corporation
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//	http ://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.
//
//*****************************************************************************

#pragma once
#include "UncompressedSampleProvider.h"

namespace winrt::FFmpegInterop::implementation
{
	class UncompressedAudioSampleProvider :
		public UncompressedSampleProvider
	{
	public:
		UncompressedAudioSampleProvider(_In_ const AVFormatContext* formatContext, _In_ AVStream* stream, _In_ Reader& reader, _In_ uint32_t allowedDecodeErrors);

		void SetEncodingProperties(_Inout_ const Windows::Media::MediaProperties::IMediaEncodingProperties& encProp, _In_ bool setFormatUserData) override;

	protected:
		void Flush() noexcept override;
		std::tuple<Windows::Storage::Streams::IBuffer, int64_t, int64_t, std::vector<std::pair<GUID, Windows::Foundation::IInspectable>>, std::vector<std::pair<GUID, Windows::Foundation::IInspectable>>> GetSampleData() override;

	private:
		// Minimum duration (in ms) for uncompressed audio samples. 
		// We'll compact shorter decoded audio samples until this threshold is reached.
		static constexpr int64_t MIN_AUDIO_SAMPLE_DUR_MS{ 200 };

		static std::tuple<AVSampleFormat, GUID> GetOutputFormat(_In_ AVSampleFormat format) noexcept;
		static bool IsSupportedFormat(_In_ AVSampleFormat format) noexcept { auto [ffmpegOutputFormat, mfOutputFormat] = GetOutputFormat(format); return format == ffmpegOutputFormat; }
		static AVSampleFormat GetFFmpegOutputFormat(_In_ AVSampleFormat format) noexcept { auto [ffmpegOutputFormat, mfOutputFormat] = GetOutputFormat(format); return ffmpegOutputFormat; }
		static GUID GetMFOutputFormat(_In_ AVSampleFormat format) noexcept { auto [ffmpegOutputFormat, mfOutputFormat] = GetOutputFormat(format); return mfOutputFormat; }
		static void InitCodecContext(_In_ AVCodecContext* codecContext) noexcept;

		bool IsUsingResampler() const noexcept { return m_swrContext != nullptr; }

		int64_t m_minAudioSampleDur{ 0 };
		AVSampleFormat m_outputFormat{ AV_SAMPLE_FMT_NONE };
		SwrContext_ptr m_swrContext;
		bool m_lastDecodeFailed{ false };
	};
}
