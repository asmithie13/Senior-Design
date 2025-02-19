//
//  ContentView.swift
//  SeniorDesign
//
//  Created by Dillon Hepler on 2/5/25.
//

import SwiftUI
import Speech

struct ContentView: View {
    @State private var gameStarted = false  // Tracks game state
    @State private var currentPlayer = "Player One"  // whose turn it is
    @State private var lastMove = ""  // Stores the last spoken move
    @State private var winner: String? = nil  // Stores the winner
    @State private var isListening = false  // Indicates if speech recognition is active
    @State private var piecesClaimedP1 = 0  // Keeps track of pieces claimed by Player One
    @State private var piecesClaimedP2 = 0  // Keeps track of pieces claimed by Player Two
    private let speechRecognizer = SpeechRecognizer()  // Handles speech recognition
    
    var body: some View {
        VStack {
            if gameStarted {
                Text("Voice Checkers")
                    .font(.largeTitle)
                    .padding()
                
                Text("Current Turn: \(currentPlayer)")  // Displays current player's turn
                    .font(.title2)
                    .padding()
                
                Text("\(currentPlayer), please say your move:")
                    .font(.headline)
                    .padding()
                
                Text("Current Move: \(lastMove)")
                    .font(.title3)
                    .padding()
                
                Button(isListening ? "Listening..." : "Speak Move") {
                    toggleListening()  // Toggles voice recognition on or off
                }
                .padding()
                .buttonStyle(.borderedProminent)
                
                Button("Send Move") {
                    sendMove()  // Sends the last move and switches turns
                }
                .padding()
                .buttonStyle(.borderedProminent)
                
                Button("Clear") {
                    clearLastMove()  // Clears the last move input
                }
                .padding()
                .buttonStyle(.borderedProminent)
                
                Button("Restart Game") {
                    endGame()  // Ends the game and determines a winner
                }
                .padding()
                .buttonStyle(.borderedProminent)
            } else {
                Text("Welcome to Voice Checkers")
                    .font(.largeTitle)
                    .padding()
                
                Button("Start Game") {
                    gameStarted = true  // Starts the game
                    currentPlayer = "Player One"
                    lastMove = ""
                    winner = nil
                }
                .padding()
                .buttonStyle(.borderedProminent)
            }
            
            if let winner = winner {
                Text("Winner: \(winner)")  // Displays the winner of the game
                    .font(.title)
                    .foregroundColor(.green)
                    .padding()
            }
        }
    }
    
    // Toggles speech recognition on or off
    func toggleListening() {
        if isListening {
            speechRecognizer.stopListening()  // Stops speech recognition
            isListening = false
        } else {
            restartListening()  // Restarts speech recognition
        }
    }
    
    // Starts listening and processes the recognized speech
    func restartListening() {
        speechRecognizer.stopListening()  // Ensures previous session stops before starting a new one
        speechRecognizer.startListening { result in
            if result == "CLEAR" {
                DispatchQueue.main.async {
                    clearLastMove()  // Clears the last move if "CLEAR" is spoken
                }
            } else if result == "SEND" {
                DispatchQueue.main.async {
                    lastMove = ""
                    currentPlayer = (currentPlayer == "Player One") ? "Player Two" : "Player One"  // Switches player turns
                }
            } else if result == "RESTART" {
                DispatchQueue.main.async {
                    endGame()  // Ends the game if "RESTART" is spoken
                }
            } else {
                let filteredMove = result.filter { $0.isNumber }.prefix(4)  // Extracts numbers from the spoken move
                if filteredMove.count == 4 {
                    let formattedMove = "\(filteredMove.prefix(2)) to \(filteredMove.suffix(2))"
                    DispatchQueue.main.async {
                        lastMove = formattedMove  // Updates the move display
                    }
                }
            }
        }
        isListening = true
    }
    
    // Sends the last move and switches turns
    func sendMove() {
        lastMove = ""
        currentPlayer = (currentPlayer == "Player One") ? "Player Two" : "Player One"
    }
    
    // Clears the last move
    func clearLastMove() {
        lastMove = ""
    }
    
    // Ends the game 
    func endGame() {
        winner = ["Player One", "Player Two"].randomElement()!
        gameStarted = false
    }
}

// Speech recognition class that listens for commands
class SpeechRecognizer: NSObject, SFSpeechRecognizerDelegate {
    private let recognizer = SFSpeechRecognizer()
    private var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?
    private var recognitionTask: SFSpeechRecognitionTask?
    private let audioEngine = AVAudioEngine()
    
    // Starts listening for spoken input
    func startListening(completion: @escaping (String) -> Void) {
        guard let recognizer = recognizer, recognizer.isAvailable else { return }
        recognitionRequest = SFSpeechAudioBufferRecognitionRequest()
        let inputNode = audioEngine.inputNode
        guard let request = recognitionRequest else { return }
        
        request.shouldReportPartialResults = true
        
        recognitionTask = recognizer.recognitionTask(with: request) { result, error in
            if let result = result {
                let recognizedText = result.bestTranscription.formattedString.lowercased()
                
                // Matches spoken commands to actions
                if recognizedText.contains("send") {
                    completion("SEND")
                } else if recognizedText.contains("clear") {
                    completion("CLEAR")
                } else if recognizedText.contains("restart") {
                    completion("RESTART")
                } else {
                    completion(recognizedText)  // Returns recognized text for move processing
                }
            }
            if error != nil {
                self.stopListening()  // Stops listening if an error occurs
            }
        }
        
        let recordingFormat = inputNode.outputFormat(forBus: 0)
        inputNode.installTap(onBus: 0, bufferSize: 1024, format: recordingFormat) { (buffer, when) in
            request.append(buffer)  // Captures audio input
        }
        
        audioEngine.prepare()
        try? audioEngine.start()  // Starts the audio engine
    }
    
    // Stops listening and clears resources
    func stopListening() {
        audioEngine.stop()
        audioEngine.inputNode.removeTap(onBus: 0)
        recognitionRequest?.endAudio()
        recognitionTask?.cancel()
    }
}

// SwiftUI preview
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
