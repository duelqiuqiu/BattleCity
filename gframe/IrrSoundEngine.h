#pragma once
#include "SoundEffectPlayer.h"
#include "irrKlang.h"
#include "../irrKlang/plugins/ikpMP3/CIrrKlangAudioStreamLoaderMP3.h"
#include <irrlicht.h>
#include "PlatformString.h"
#include <memory>
using namespace irrklang;
namespace ygo
{
	class CMyReadFile : public irrklang::IFileReader
	{
		irr::io::IReadFile* _file;
	public:
		CMyReadFile(irr::io::IReadFile* file)
		{
			_file = file;
		}

		virtual ~CMyReadFile()
		{
			_file->drop();
		}

		//! Reads an amount of bytes from the file.
		//! \param buffer: Pointer to buffer where to read bytes will be written to.
		//! \param sizeToRead: Amount of bytes to read from the file.
		//! \return Returns how much bytes were read.
		virtual ik_s32 read(void* buffer, ik_u32 sizeToRead)
		{
			return _file->read(buffer, sizeToRead);
		}

		//! Changes position in file, returns true if successful.
		//! \param finalPos: Destination position in the file.
		//! \param relativeMovement: If set to true, the position in the file is
		//! changed relative to current position. Otherwise the position is changed 
		//! from beginning of file.
		//! \return Returns true if successful, otherwise false.
		virtual bool seek(ik_s32 finalPos, bool relativeMovement = false)
		{
			return _file->seek(finalPos, relativeMovement);
		}

		//! Returns size of file.
		//! \return Returns the size of the file in bytes.
		virtual ik_s32 getSize()
		{
			return _file->getSize();
		}

		//! Returns the current position in the file.
		//! \return Returns the current position in the file in bytes.
		virtual ik_s32 getPos()
		{
			return _file->getPos();
		}

		//! Returns name of file.
		//! \return Returns the file name as zero terminated character string.
		virtual const ik_c8* getFileName()
		{
			return PlatformString::FromPath(_file->getFileName()).ToUTF8().c_str();
		}


	};

	// a class implementing the IFileFactory 
	// interface to override irrklang file access
	class CMyFileFactory : public irrklang::IFileFactory
	{
		irr::io::IFileSystem* _fileSystem;
	public:
		CMyFileFactory(irr::io::IFileSystem* fileSystem)
		{
			_fileSystem = fileSystem;
			_fileSystem->grab();
		}
		virtual ~CMyFileFactory()
		{
			_fileSystem->drop();
		}

		//! Opens a file for read access. Simply return 0 if file not found.
		virtual irrklang::IFileReader* createFileReader(const ik_c8* filename)
		{
			printf("MyFileFactory: open file %s\n", filename);
			auto file = _fileSystem->createAndOpenFile(PlatformString::FromUTF8(filename).ToPath());
			return new CMyReadFile(file);
		}


	protected:
	};


	class MyIrrSound : public MyISound
	{
		ISound* _sound;
	public:
		MyIrrSound(ISound* sound)
		{
			_sound = sound;
		}

		~MyIrrSound()
		{
			_sound->drop();
		}

		virtual bool GetIsFinished() override
		{
			return _sound->isFinished();
		}

		virtual bool GetIsPaused() override
		{
			return _sound->getIsPaused();
		}

		virtual void Stop() override
		{
			return _sound->stop();
		}
	};


	class IrrSoundEngine :public  MyISoundEngine
	{
		ISoundEngine* _engine = nullptr;
	public:
		IrrSoundEngine(irr::io::IFileSystem* fileSystem)
		{
			_engine = createIrrKlangDevice();
			auto factory = new CMyFileFactory(fileSystem);
			_engine->addFileFactory(factory);
			factory->drop();

			CIrrKlangAudioStreamLoaderMP3* loader = new CIrrKlangAudioStreamLoaderMP3();
			_engine->registerAudioStreamLoader(loader);
			loader->drop();
		}

		virtual std::shared_ptr<MyISound> Play(const PlatformString& filename) override
		{
			auto sound = _engine->play2D(filename.ToUTF8().c_str());
			return std::make_shared<EmptySound>();
		}

		virtual void SetSoundVolume(float val) override
		{
			_engine->setSoundVolume(val);
		}

		virtual std::shared_ptr<ygo::MyISound> PlayAsTrack(const PlatformString& filename) override
		{
			auto sound = _engine->play2D(filename.ToUTF8().c_str(), false, false, true);
			return std::make_shared<MyIrrSound>(sound);
		}

		virtual ~IrrSoundEngine()
		{
			_engine->drop();
		}
	};

}