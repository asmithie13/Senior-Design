//
//  ContentView.swift
//  SeniorDesign
//
//  Created by Dillon Hepler on 2/5/25.
//


import SwiftUI
import Speech

struct ContentView: View {
    // MARK: - Game States
    @State private var gameStarted = false
    @State private var currentPlayer = "Player One"
    @State private var lastMove = ""
    @State private var winner: String? = nil
    @State private var isListening = false
    
    // MARK: - Speech/Mic Permissions
    @State private var speechAuthorizationStatus = SFSpeechRecognizerAuthorizationStatus.notDetermined
    @State private var micPermissionGranted = false
    
    // MARK: - Network Config
   
    
    private let speechRecognizer = SpeechRecognizer()
    
    var body: some View {
        VStack {
            if gameStarted {
                Text("Voice Checkers")
                    .font(.largeTitle)
                    .padding()
                
                Text("Current Turn: \(currentPlayer)")
                    .font(.title2)
                    .padding()
                
                Text("\(currentPlayer), please say your move:")
                    .font(.headline)
                    .padding()
                
                Text("Current Move: \(lastMove)")
                    .font(.title3)
                    .padding()
                
                Button(isListening ? "Listening..." : "Speak Move") {
                    toggleListening()
                }
                .padding()
                .buttonStyle(.borderedProminent)
                .disabled(!canUseSpeech)  // Only enable if speech is authorized
                
                Button("Send Move") {
                    sendMove()
                }
                .padding()
                .buttonStyle(.borderedProminent)
                
                Button("Clear") {
                    clearLastMove()
                }
                .padding()
                .buttonStyle(.borderedProminent)
                
                Button("Restart Game") {
                    endGame()
                }
                .padding()
                .buttonStyle(.borderedProminent)
                
                Divider().padding()
                
                // Example extra buttons for GET/POST test
                Button("GET Data from Pi") {
                    getDataFromServer()
                }
                .padding()
                
                Button("POST Dummy Data") {
                    let dummyData: [String: Any] = ["status": "testing", "move": "dummy"]
                    sendDataToServer(data: dummyData)
                }
                .padding()
                
            } else {
                Text("Welcome to Voice Checkers")
                    .font(.largeTitle)
                    .padding()
                
                Button("Start Game") {
                    startGame()
                }
                .padding()
                .buttonStyle(.borderedProminent)
            }
            
            if let winner = winner {
                Text("Winner: \(winner)")
                    .font(.title)
                    .foregroundColor(.green)
                    .padding()
            }
        }
        .onAppear {
            requestPermissions()
        }
    }
    
    // MARK: - Permission Check
    private var canUseSpeech: Bool {
        return (speechAuthorizationStatus == .authorized) && micPermissionGranted
    }
    
    // MARK: - Game Logic
    
    func startGame() {
        gameStarted = true
        currentPlayer = "Player One"
        lastMove = ""
        winner = nil
    }
    
    func endGame() {
        // For now, pick a random winner
        winner = ["Player One", "Player Two"].randomElement()!
        gameStarted = false
    }
    
    func clearLastMove() {
        lastMove = ""
    }
    
    func sendMove() {
        // Example: "12 to 13" => "1213"
        let cleanedMove = lastMove.replacingOccurrences(of: " to ", with: "")
        
        // Switch turn first or after server response? Your choice.
        lastMove = ""
        currentPlayer = (currentPlayer == "Player One") ? "Player Two" : "Player One"
        
        // Send the move to the Pi (HTTP POST)
        let dataToSend: [String: Any] = ["move": cleanedMove]
        sendDataToServer(data: dataToSend)
    }
    
    // MARK: - Speech Handling
    
    func toggleListening() {
        guard canUseSpeech else { return }
        
        if isListening {
            speechRecognizer.stopListening()
            isListening = false
        } else {
            restartListening()
        }
    }
    
    func restartListening() {
        guard canUseSpeech else { return }
        
        speechRecognizer.stopListening()
        speechRecognizer.startListening { result in
            // Basic commands
            if result == "CLEAR" {
                DispatchQueue.main.async {
                    clearLastMove()
                }
            } else if result == "SEND" {
                DispatchQueue.main.async {
                    // Actually do a move send if you want
                    sendMove()
                }
            } else if result == "RESTART" {
                DispatchQueue.main.async {
                    endGame()
                }
            } else {
                // Parse numeric moves
                let filteredMove = result.filter { $0.isNumber }.prefix(4)
                if filteredMove.count == 4 {
                    let formattedMove = "\(filteredMove.prefix(2)) to \(filteredMove.suffix(2))"
                    DispatchQueue.main.async {
                        lastMove = formattedMove
                    }
                }
            }
        }
        isListening = true
    }
    
    // MARK: - Permissions
    
    func requestPermissions() {
        // Ask for Speech Recognition
        SFSpeechRecognizer.requestAuthorization { status in
            DispatchQueue.main.async {
                self.speechAuthorizationStatus = status
            }
        }
        // Ask for Mic
        AVAudioSession.sharedInstance().requestRecordPermission { granted in
            DispatchQueue.main.async {
                self.micPermissionGranted = granted
            }
        }
    }
    
    // MARK: - HTTP Networking
    
    // Example: GET request
    func getDataFromServer() {
        guard let url = URL(string: "http://172.20.10.6:5000/send_data") else {
            return
        }
        
        let task = URLSession.shared.dataTask(with: url) { data, response, error in
            if let error = error {
                print("GET request error: \(error.localizedDescription)")
                return
            }
            if let data = data {
                do {
                    if let jsonResponse = try JSONSerialization.jsonObject(with: data, options: []) as? [String: Any] {
                        print("GET response: \(jsonResponse)")
                    }
                } catch {
                    print("JSON parse error: \(error.localizedDescription)")
                }
            }
        }
        task.resume()
    }
    
    // Example: POST request
    func sendDataToServer(data: [String: Any]) {
        guard let url = URL(string: "http://172.20.10.6:5000/send_data") else {
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        
        do {
            let jsonData = try JSONSerialization.data(withJSONObject: data, options: [])
            request.httpBody = jsonData
        } catch {
            print("Error serializing JSON: \(error.localizedDescription)")
            return
        }
        
        let task = URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                print("POST request error: \(error.localizedDescription)")
                return
            }
            if let data = data {
                do {
                    if let jsonResponse = try JSONSerialization.jsonObject(with: data, options: []) as? [String: Any] {
                        print("POST response: \(jsonResponse)")
                    }
                } catch {
                    print("Error parsing JSON: \(error.localizedDescription)")
                }
            }
        }
        task.resume()
    }
}

// MARK: - SpeechRecognizer Class

class SpeechRecognizer: NSObject, SFSpeechRecognizerDelegate {
    private let recognizer = SFSpeechRecognizer()
    private var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?
    private var recognitionTask: SFSpeechRecognitionTask?
    private let audioEngine = AVAudioEngine()
    
    func startListening(completion: @escaping (String) -> Void) {
        guard let recognizer = recognizer, recognizer.isAvailable else { return }
        recognitionRequest = SFSpeechAudioBufferRecognitionRequest()
        guard let request = recognitionRequest else { return }
        
        let inputNode = audioEngine.inputNode
        request.shouldReportPartialResults = true
        
        recognitionTask = recognizer.recognitionTask(with: request) { result, error in
            if let result = result {
                let recognizedText = result.bestTranscription.formattedString.lowercased()
                
                if recognizedText.contains("send") {
                    completion("SEND")
                } else if recognizedText.contains("clear") {
                    completion("CLEAR")
                } else if recognizedText.contains("restart") {
                    completion("RESTART")
                } else {
                    completion(recognizedText)
                }
            }
            if error != nil {
                self.stopListening()
            }
        }
        
        let recordingFormat = inputNode.outputFormat(forBus: 0)
        inputNode.installTap(onBus: 0, bufferSize: 1024, format: recordingFormat) { buffer, _ in
            request.append(buffer)
        }
        
        audioEngine.prepare()
        do {
            try audioEngine.start()
        } catch {
            print("Audio engine couldn't start. Error: \(error.localizedDescription)")
            stopListening()
        }
    }
    
    func stopListening() {
        audioEngine.stop()
        audioEngine.inputNode.removeTap(onBus: 0)
        recognitionRequest?.endAudio()
        recognitionTask?.cancel()
    }
}

// MARK: - SwiftUI Preview

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
