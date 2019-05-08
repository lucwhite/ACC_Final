#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sampleRate = 44100;
    channels = 2;
    
    v1.open(ofToDataPath("testMovie.mov"), ofFile::ReadWrite, false);
    v2.open(ofToDataPath("testMovie2.mov"), ofFile::ReadWrite, false);
    v3.open(ofToDataPath("testMovie3.mov"), ofFile::ReadWrite, false);
    v4.open(ofToDataPath("testMovie4.mov"), ofFile::ReadWrite, false);
    
    
    v1.remove();
    v2.remove();
    v3.remove();
    v4.remove();
    
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    vidGrabber.setDesiredFrameRate(30);
    vidGrabber.initGrabber(640, 480);
    //    vidRecorder.setFfmpegLocation(ofFilePath::getAbsolutePath("ffmpeg")); // use this is you have ffmpeg installed in your data folder
    
    fileName = "testMovie";
    fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
    
    vidRecorder.setVideoCodec("mpeg4");
    vidRecorder.setVideoBitrate("800k");
    vidRecorder.setAudioCodec("mp3");
    vidRecorder.setAudioBitrate("192k");
    
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    
    //    soundStream.listDevices();
    //    soundStream.setDeviceID(11);
    soundStream.setup(this, 0, channels, sampleRate, 256, 4);
    
    ofSetWindowShape(vidGrabber.getWidth(), vidGrabber.getHeight()    );
    bRecording = false;
    ofEnableAlphaBlending();
    
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
}

//--------------------------------------------------------------
void ofApp::update(){
    if(vid.isPlaying() && vid.isLoaded()){
       vid.update();
    }
    if(vid2.isPlaying() && vid2.isLoaded()){
        vid2.update();
    }
    if(vid3.isPlaying() && vid3.isLoaded()){
        vid3.update();
    }
    if(vid4.isPlaying() && vid4.isLoaded()){
        vid4.update();
    }
    vidGrabber.update();
//    if(vidGrabber.isFrameNew() && bRecording && vid.isPlaying()){
//
//        bool success = vidRecorder.addFrame(vidGrabber.getPixels());
//        if (!success) {
//            ofLogWarning("This frame was not added!");
//        }
//        fCount += 1;
//        if(fCount == vid.getTotalNumFrames()){
//            bRecording = false;
//            vidRecorder.close();
//            keyReleased(99);
//            fCount = 0;
//        }
//    }
    if(vidGrabber.isFrameNew() && bRecording){
        bool success = vidRecorder.addFrame(vidGrabber.getPixels());
        if (!success) {
            ofLogWarning("This frame was not added!");
        }
    }
    
    // Check if the video recorder encountered any error while writing video frame or audio smaples.
    if (vidRecorder.hasVideoError()) {
        ofLogWarning("The video recorder failed to write some frames!");
    }
    
    if (vidRecorder.hasAudioError()) {
        ofLogWarning("The video recorder failed to write some audio samples!");
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(vid.isPlaying() && vid.isLoaded()){
        vid.draw(0,0);
        //ofLogNotice("1");
    }else if(vid2.isPlaying() && vid2.isLoaded()){
        vid2.draw(100,100);
        //ofLogNotice("2");
    }else if(vid3.isPlaying() && vid3.isLoaded()){
        vid3.draw(100,100);
        //ofLogNotice("3");
    }else if(vid4.isPlaying() && vid4.isLoaded()){
        vid4.draw(100,100);
        //ofLogNotice("4");
    }else{
        vidGrabber.draw(0,0);
        stringstream ss;
        ss << "video queue size: " << vidRecorder.getVideoQueueSize() << endl
        << "audio queue size: " << vidRecorder.getAudioQueueSize() << endl
        << "FPS: " << ofGetFrameRate() << endl
        << (bRecording?"pause":"start") << " recording: r" << endl
        << (bRecording?"close current video file: c":"") << endl;
        
        ofSetColor(0,0,0,100);
        ofDrawRectangle(0, 0, 260, 75);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(ss.str(),15,15);
    }
    
    ofSetColor(255, 255, 255);
    
    
    
    
    if(bRecording){
        ofSetColor(255, 0, 0);
        ofDrawCircle(ofGetWidth() - 20, 20, 5);
    }
    
    ofSetColor(0,0,0);
   
}

//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int nChannels){
    if(bRecording)
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

void ofApp::stopRec(){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            vidRecorder.setup(fileName+fileExt, vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels);
            //          vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, vidGrabber.getWidth(), vidGrabber.getHeight(), 30); // no audio
            //            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 0,0,0, sampleRate, channels); // no video
            //          vidRecorder.setupCustomOutput(vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels, "-vcodec mpeg4 -b 1600k -acodec mp2 -ab 128k -f mpegts udp://localhost:1234"); // for custom ffmpeg output string (streaming, etc)
            
            // Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
    if(key=='c'){
        bRecording = false;
        vidRecorder.close();
//        ofVideoPlayer player;
//        if(player.load("testMovie" + ofToString(pCount) + ".mov")){
//            player.play();
//            pCount++;
//            fileName = "testMovie" + ofToString(pCount);
//            fileExt = ".mov";
//        }
//        players.push_back(player);
//        ofLogNotice() << ofToString(players.size()) << endl;
        
        if(vid.load("testMovie.mov"));
            vid.play();
            fileName = "testMovie2";
            fileExt = ".mov";
        if(vid2.load("testMovie2.mov")){
            vid.play();
            vid2.play();
            fileName = "testMovie3";
            fileExt = ".mov";
        }
        if(vid3.load("testMovie3.mov")){
            vid.play();
            vid2.play();
            vid3.play();
            fileName = "testMovie4";
            fileExt = ".mov";
        }
        if(vid4.load("testMovie4.mov")){
            vid.play();
            vid2.play();
            vid3.play();
            vid4.play();
            fileName = "testMovie";
            fileExt = ".mov";
        }
        
    }
    if(key=='d'){
        v1.open(ofToDataPath("testMovie.mov"), ofFile::ReadWrite, false);
        v2.open(ofToDataPath("testMovie2.mov"), ofFile::ReadWrite, false);
        v3.open(ofToDataPath("testMovie3.mov"), ofFile::ReadWrite, false);
        v4.open(ofToDataPath("testMovie4.mov"), ofFile::ReadWrite, false);
        
        vid.stop();
        vid2.stop();
        vid3.stop();
        vid4.stop();
        
        
        
        v1.remove();
        v2.remove();
        v3.remove();
        v4.remove();
        
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
