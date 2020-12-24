#include "CanAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "CanAnalyzer.h"
#include "CanAnalyzerSettings.h"
#include <iostream>
#include <sstream>
#include <bitset>
#include <iomanip>

#pragma warning(disable: 4800) //warning C4800: 'U64' : forcing value to bool 'true' or 'false' (performance warning)

std::string GetBinaryStringFromHexString (std::string sHex){
    std::string sReturn = "";
    for (int i = 2; i < sHex.length (); ++i)
    {
        switch (sHex [i])
        {
            case '0': sReturn.append ("0000"); break;
            case '1': sReturn.append ("0001"); break;
            case '2': sReturn.append ("0010"); break;
            case '3': sReturn.append ("0011"); break;
            case '4': sReturn.append ("0100"); break;
            case '5': sReturn.append ("0101"); break;
            case '6': sReturn.append ("0110"); break;
            case '7': sReturn.append ("0111"); break;
            case '8': sReturn.append ("1000"); break;
            case '9': sReturn.append ("1001"); break;
            case 'a': sReturn.append ("1010"); break;
            case 'b': sReturn.append ("1011"); break;
            case 'c': sReturn.append ("1100"); break;
            case 'd': sReturn.append ("1101"); break;
            case 'e': sReturn.append ("1110"); break;
            case 'f': sReturn.append ("1111"); break;
        }
    }
    return sReturn;
}

std::string MessageType(std::string input){
    std::string output;
    if (input=="00"){output="RD";}
    else if (input=="01"){output="WR";}
    else if (input=="10"){output="RR";}
    else if (input=="11"){output="AK";}
    return output;
}

std::stringstream conversion (char* in){
    std::stringstream out;
    out<<in;
    std::string binary;
    binary=GetBinaryStringFromHexString (out.str());
    std::string page=binary.substr(24,8);
    std::string message=binary.substr(17,7);
    std::string device=binary.substr(8,8);
    std::string type=MessageType(binary.substr(6,2));
    std::string I;
    if (binary.substr()=="1"){
        I="I";
    } else {
        I="B";
    }
    std::stringstream outstring;
    outstring<<type;
    outstring<<" D: ";
    std::bitset<8> set(device);
    outstring<<std::setfill('0')<<std::setw(2)<<std::hex<<set.to_ulong();
    outstring<<" ";
    outstring<<I;
    outstring<<" M: ";
    std::bitset<7> set2(message);
    outstring<<std::setfill('0')<<std::setw(2)<<std::hex<<set2.to_ulong();
    outstring<<" P: ";
    std::bitset<8> set3(page);
    outstring<<std::setfill('0')<<std::setw(2)<<std::hex<<set3.to_ulong();
    return outstring;
}

std::stringstream shortconversion (char* in){
    std::stringstream out;
    out<<in;
    std::string binary;
    binary=GetBinaryStringFromHexString (out.str());
    std::string page=binary.substr(24,8);
    std::string message=binary.substr(17,7);
    std::string device=binary.substr(8,8);
    std::string type=MessageType(binary.substr(6,2));
    std::string I;
    if (binary.substr()=="1"){
        I="I";
    } else {
        I="B";
    }
    std::stringstream outstring;
    outstring<<type;
    outstring<<" D: ";
    std::bitset<8> set(device);
    outstring<<std::setfill('0')<<std::setw(2)<<std::hex<<set.to_ulong();
    outstring<<" M: ";
    std::bitset<7> set2(message);
    outstring<<std::setfill('0')<<std::setw(2)<<std::hex<<set2.to_ulong();
    return outstring;
}

CanAnalyzerResults::CanAnalyzerResults( CanAnalyzer* analyzer, CanAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

CanAnalyzerResults::~CanAnalyzerResults()
{
}

void CanAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& /*channel*/, DisplayBase display_base )  //unrefereced vars commented out to remove warnings.
{
	//we only need to pay attention to 'channel' if we're making bubbles for more than one channel (as set by AddChannelBubblesWillAppearOn)
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	switch( frame.mType )
	{
	case IdentifierField:
	case IdentifierFieldEx:
		{
			char numbers[128];

			if( frame.mType == IdentifierField )
				AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 12, numbers, 128 );
			else
				AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 32, numbers, 128 );

            std::stringstream number=shortconversion (numbers);
            std::string number_str = number.str();

			std::stringstream ss;

			AddResultString( "Id" );

            ss << number_str;
			AddResultString( ss.str().c_str() );
			ss.str("");

            number=shortconversion (numbers);
            number_str = number.str();
            ss << number_str;
            AddResultString( ss.str().c_str() );
            ss.str("");

			if( frame.HasFlag( REMOTE_FRAME ) == false )
			{
				if( frame.mType == IdentifierField )
					ss << "Standard CAN Identifier: " << number_str;
				else
					ss <<number_str;
			}else
			{
				if( frame.mType == IdentifierField )
					ss << "Standard CAN Identifier: " << number_str << " (RTR)";
				else
					ss << "Extended CAN Identifier: " << number_str << " (RTR)";
			}

			AddResultString( ss.str().c_str() );
		}
		break;
	case ControlField:
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 4, number_str, 128 );

			std::stringstream ss;
			AddResultString( "Ctrl" );

			ss << "Ctrl: " << number_str;
			AddResultString( ss.str().c_str() );
			ss.str("");

			ss << "Control Field: " << number_str;
			AddResultString( ss.str().c_str() );

			ss << " bytes";
			AddResultString( ss.str().c_str() );
		}
		break;
	case DataField:
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

			AddResultString( number_str );

			std::stringstream ss;
			ss << "Data: " << number_str;
			AddResultString( ss.str().c_str() );
			ss.str("");

			ss << "Data Field Byte: " << number_str;
			AddResultString( ss.str().c_str() );
		}
		break;
	case CrcField:
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 15, number_str, 128 );

			AddResultString( "CRC" );

			std::stringstream ss;
			ss << "CRC: " << number_str;
			AddResultString( ss.str().c_str() );
			ss.str("");

			ss << "CRC value: " << number_str;
			AddResultString( ss.str().c_str() );
		}
		break;	
	case AckField:
		{
			if( bool( frame.mData1 ) == true )
				AddResultString( "ACK" );
			else
				AddResultString( "NAK" );
		}
		break;	
	case CanError:
		{
			AddResultString( "E" );
			AddResultString( "Error" );
		}
		break;
	}
}


void CanAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 /*export_type_user_id*/ )
{
	//export_type_user_id is only important if we have more than one export type.
	std::stringstream ss;
	void* f = AnalyzerHelpers::StartFile( file );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	ss << "Time [s],Packet,Type,Identifier,Control,Data,CRC,ACK" << std::endl;
	U64 num_frames = GetNumFrames();
	U64 num_packets = GetNumPackets();
	for( U32 i=0; i < num_packets; i++ )
	{
		if( i != 0 )
		{
			//below, we "continue" the loop rather than run to the end.  So we need to save to the file here.
			ss << std::endl;

			AnalyzerHelpers::AppendToFile( (U8*)ss.str().c_str(), ss.str().length(), f );
			ss.str( std::string() );


			if( UpdateExportProgressAndCheckForCancel( i, num_packets ) == true )
			{
				AnalyzerHelpers::EndFile( f );
				return;
			}
		}


		U64 first_frame_id;
		U64 last_frame_id;
		GetFramesContainedInPacket( i, &first_frame_id, &last_frame_id );
		Frame frame = GetFrame( first_frame_id );
		
		//static void GetTimeString( U64 sample, U64 trigger_sample, U32 sample_rate_hz, char* result_string, U32 result_string_max_length );
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char packet_str[128];
		AnalyzerHelpers::GetNumberString( i, Decimal, 0, packet_str, 128 );

		if( frame.HasFlag( REMOTE_FRAME ) == false )
			ss << time_str << "," << packet_str << ",DATA";
		else
			ss << time_str << "," << packet_str << ",REMOTE";
		
		U64 frame_id = first_frame_id;

		frame = GetFrame( frame_id );

		char number_str[128];

		if( frame.mType == IdentifierField )
		{
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 12, number_str, 128);
			ss << "," << number_str;
			++frame_id;
		}
		else if( frame.mType == IdentifierFieldEx )
		{
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 32, number_str, 128 );
			ss << "," << number_str;
			++frame_id;
		}
		else
		{
			ss << ",";
		}

		if( frame_id > last_frame_id )
			continue;

		frame = GetFrame( frame_id );
		if( frame.mType == ControlField )
		{
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 4, number_str, 128);
			ss << "," << number_str;
			++frame_id;
		}
		else
		{
			ss << ",";
		}
		ss << ",";
		if( frame_id > last_frame_id )
			continue;

		for( ; ; )
		{
			frame = GetFrame( frame_id );
			if( frame.mType != DataField )
				break;

			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
			ss << number_str;
			if( frame_id == last_frame_id )
				break;

			++frame_id;
			if( GetFrame( frame_id ).mType == DataField )
				ss << " ";
		}

		if( frame_id > last_frame_id )
			continue;

		frame = GetFrame( frame_id );
		if( frame.mType == CrcField )
		{
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 15, number_str, 128);
			ss << "," << number_str;
			++frame_id;
		}else
		{
			ss << ",";
		}
		if( frame_id > last_frame_id )
			continue;

		frame = GetFrame( frame_id );
		if( frame.mType == AckField )
		{
			if( bool( frame.mData1 ) == true )
				ss << "," << "ACK";
			else
				ss << "," << "NAK";

			++frame_id;
		}else
		{
			ss << ",";
		}
	}

	UpdateExportProgressAndCheckForCancel( num_frames, num_frames );
	AnalyzerHelpers::EndFile( f );
}

void CanAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
    ClearTabularText();

	Frame frame = GetFrame( frame_index );

	switch( frame.mType )
	{
	case IdentifierField:
	case IdentifierFieldEx:
		{
			char number_str[128];

			if( frame.mType == IdentifierField )
				AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 12, number_str, 128 );
			else
				AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 32, number_str, 128 );

			std::stringstream ss;

			
			if( frame.HasFlag( REMOTE_FRAME ) == false )
			{
				if( frame.mType == IdentifierField )
					ss << "Standard CAN Identifier: " << number_str;
				else
					ss << "Extended CAN Identifier: " << number_str;
			}else
			{
				if( frame.mType == IdentifierField )
					ss << "Standard CAN Identifier: " << number_str << " (RTR)";
				else
					ss << "Extended CAN Identifier: " << number_str << " (RTR)";
			}

			AddTabularText( ss.str().c_str() );
		}
		break;
	case ControlField:
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 4, number_str, 128 );

			std::stringstream ss;
			
			ss << "Control Field: " << number_str;
			ss << " bytes";
			AddTabularText( ss.str().c_str() );

		}
		break;
	case DataField:
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

			std::stringstream ss;
			
			ss << "Data Field Byte: " << number_str;
			AddTabularText( ss.str().c_str() );
		}
		break;
	case CrcField:
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 15, number_str, 128 );
						
			std::stringstream ss;
			
			ss << "CRC value: " << number_str;
			AddTabularText( ss.str().c_str() );
		}
		break;	
	case AckField:
		{
			if( bool( frame.mData1 ) == true )
				AddTabularText( "ACK" );
			else
				AddTabularText( "NAK" );
		}
		break;	
	case CanError:
		{
			AddTabularText( "Error" );
		}
		break;
	}
}

void CanAnalyzerResults::GeneratePacketTabularText( U64 /*packet_id*/, DisplayBase /*display_base*/ )  //unrefereced vars commented out to remove warnings.
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void CanAnalyzerResults::GenerateTransactionTabularText( U64 /*transaction_id*/, DisplayBase /*display_base*/ )  //unrefereced vars commented out to remove warnings.
{
	ClearResultStrings();
	AddResultString( "not supported" );
}
