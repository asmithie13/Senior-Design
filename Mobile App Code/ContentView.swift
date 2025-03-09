//
//  ContentView.swift
//  SeniorDesign
//
//  Created by Dillon Hepler on 2/5/25.
//

import SwiftUI
import Speech
import AVFoundation

// MARK: - Test Bench
// Simulates different game scenarios for testing
class TestBench {
    static let shared = TestBench()
    
    // Available test scenarios
    enum TestScenario: String, CaseIterable {
        case validMove = "Valid Move"
        case invalidMove = "Invalid Move"
        case doubleJump = "Double Jump"
        case connectionError = "Connection Error"
    }
    
    private(set) var isActive = false
    private var currentScenario: TestScenario = .validMove
    
    // Structure for mock server responses
    struct MockResponse {
        let status: Bool    // Whether the move was successful
        let message: String // Response message
    }
    
    func activate(scenario: TestScenario) {
        isActive = true
        currentScenario = scenario
    }
    
    func deactivate() {
        isActive = false
    }
    
    // Generates mock responses based on selected scenario
    func getMockResponse(for move: String) -> MockResponse {
        switch currentScenario {
        case .validMove:
            return MockResponse(status: true, message: "Valid move: \(move)")
        case .invalidMove:
            return MockResponse(status: false, message: "Invalid move: \(move)")
        case .doubleJump:
            return MockResponse(status: true, message: "Double jump possible at \(move)")
        case .connectionError:
            return MockResponse(status: false, message: "Connection failed for \(move)")
        }
    }
}

// MARK: - Speech Recognizer
// Handles voice input processing using Apple's Speech framework
class SpeechRecognizer: NSObject, SFSpeechRecognizerDelegate {
    private let recognizer = SFSpeechRecognizer()
    private var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?
    private var recognitionTask: SFSpeechRecognitionTask?
    private let audioEngine = AVAudioEngine()
    
    // Starts listening for voice commands and processes results
    func startListening(completion: @escaping (String) -> Void) {
        guard let recognizer = recognizer, recognizer.isAvailable else { return }
        recognitionRequest = SFSpeechAudioBufferRecognitionRequest()
        guard let request = recognitionRequest else { return }
        
        let inputNode = audioEngine.inputNode
        request.shouldReportPartialResults = true
        
        // Process recognition results
        recognitionTask = recognizer.recognitionTask(with: request) { (result, error) in
            if let result = result {
                let recognizedText = result.bestTranscription.formattedString.lowercased()
                
                // Handle command keywords
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
        
        // Configure audio input
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
    
    // Stops voice recognition and cleans up
    func stopListening() {
        audioEngine.stop()
        audioEngine.inputNode.removeTap(onBus: 0)
        recognitionRequest?.endAudio()
        recognitionTask?.cancel()
    }
}

// MARK: - Main Content View
/// Primary game interface view
struct ContentView: View {
    // MARK: - Game States
    @State private var gameStarted = false          // Tracks if game is active
    @State private var currentPlayer = "Player One" // Current player's turn
    @State private var lastMove = ""                // Last spoken move
    @State private var winner: String? = nil        // Winner declaration
    @State private var isListening = false          // Voice input state
    @State private var serverIP = UserDefaults.standard.string(forKey: "serverIP") ?? "172.20.10.6" // Server address
    @State private var moveHistory: [MoveEntry] = [] // History of moves
    @State private var useTestBench = false          // Test mode toggle
    @State private var testScenario = TestBench.TestScenario.validMove // Selected test scenario
    
    // MARK: - Speech/Mic Permissions
    @State private var speechAuthorizationStatus = SFSpeechRecognizerAuthorizationStatus.notDetermined
    @State private var micPermissionGranted = false
    
    // MARK: - Network Config
    private let speechRecognizer = SpeechRecognizer() // Voice processor instance
    
    // Custom color definitions
    private let checkerRed = Color(red: 0.76, green: 0.16, blue: 0.19)
    private let checkerBeige = Color(red: 0.98, green: 0.96, blue: 0.84)
    private let checkerBlack = Color.black
    
    var body: some View {
        ZStack {
            // Background color
            checkerRed.ignoresSafeArea()
            
            // Main vertical stack
            VStack(spacing: 20) {
                if gameStarted {
                    gameInProgressView
                } else {
                    welcomeView
                }
                
                // Winner announcement
                if let winner = winner {
                    winnerView(winner: winner)
                }
                
                // Move history panel
                moveHistoryView
            }
            .padding()
        }
        .onAppear {
            requestPermissions() // Request mic/speech permissions on launch
        }
    }
    
    // MARK: - Game Views
    
    // Active game interface components
    private var gameInProgressView: some View {
        Group {
            // Game title
            Text("VOICE CHECKERS")
                .font(.system(size: 28, weight: .bold, design: .rounded))
                .foregroundColor(checkerBeige)
                .padding(.top, 35) // Extra padding for phone notch areas
                .padding(.bottom, 10)
            
            // Game board
            CheckerboardView()
                .frame(height: 300)
                .padding(.vertical)
            
            // Game controls
            VStack(spacing: 15) {
                currentPlayerTurnView
                moveInputSection
                actionButtons
            }
            .padding()
            .background(RoundedRectangle(cornerRadius: 15).fill(checkerRed.opacity(0.9)))
            .shadow(radius: 5)
        }
    }
    
    // Current player display
    private var currentPlayerTurnView: some View {
        VStack {
            Text("Current Turn")
                .font(.caption)
                .foregroundColor(checkerBeige)
            Text(currentPlayer)
                .font(.system(.title2, weight: .bold))
                .foregroundColor(.white)
        }
    }
    
    // Move input section with voice button
    private var moveInputSection: some View {
        VStack(spacing: 10) {
            Text("Current Move:")
                .font(.subheadline)
                .foregroundColor(checkerBeige)
            
            // Move display text
            Text(lastMove.isEmpty ? "Waiting..." : lastMove)
                .font(.system(.title3, weight: .medium))
                .foregroundColor(.white)
                .frame(minWidth: 200, minHeight: 44)
                .background(checkerRed)
                .cornerRadius(8)
                .overlay(
                    RoundedRectangle(cornerRadius: 8)
                        .stroke(checkerBeige, lineWidth: 1)
                )
            
            // Voice input button
            Button(action: toggleListening) {
                HStack {
                    Image(systemName: isListening ? "waveform" : "mic.fill")
                    Text(isListening ? "Listening..." : "Speak Move")
                        .font(.system(.title3, weight: .medium)) // Increased font size
                }
                .buttonStyle(PrimaryButtonStyle(isEnabled: canUseSpeech && !isListening))
            }
            .disabled(!canUseSpeech || isListening)
        }
    }
    
    // Action button row
    private var actionButtons: some View {
        HStack(spacing: 15) {
            // Send move button
            Button("Send Move") {
                sendMove()
            }
            .buttonStyle(PrimaryButtonStyle())
            
            // Clear move button
            Button(action: clearLastMove) {
                HStack {
                    Image(systemName: "xmark")
                    Text("Clear")
                }
            }
            .buttonStyle(SecondaryButtonStyle())
            
            // Game reset button
            Button(action: endGame) {
                HStack {
                    Image(systemName: "arrow.clockwise")
                    Text("Reset")
                }
            }
            .buttonStyle(SecondaryButtonStyle())
        }
    }
    
    // Welcome/start screen
    private var welcomeView: some View {
        VStack(spacing: 30) {
            Text("Welcome to")
                .font(.largeTitle)
                .foregroundColor(checkerBeige)
            
            // Game title
            Text("VOICE CHECKERS")
                .font(.system(size: 40, weight: .heavy, design: .rounded))
                .foregroundColor(.white)
                .shadow(color: .black.opacity(0.2), radius: 2, x: 0, y: 2)
            
            // Test bench toggle
            Toggle("Use Test Bench", isOn: $useTestBench)
                .padding()
                .foregroundColor(checkerBeige)
            
            // Test scenario picker
            if useTestBench {
                Picker("Test Scenario", selection: $testScenario) {
                    ForEach(TestBench.TestScenario.allCases, id: \.self) { scenario in
                        Text(scenario.rawValue).tag(scenario)
                    }
                }
                .pickerStyle(MenuPickerStyle())
                .padding()
            }
            
            // Server IP input
            TextField("Server IP Address", text: $serverIP)
                .disabled(useTestBench)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .padding()
                .onChange(of: serverIP) { newValue in
                    UserDefaults.standard.set(newValue, forKey: "serverIP")
                }
            
            // Start game button
            Button(action: startGame) {
                Text("START GAME")
                    .font(.title2)
                    .fontWeight(.bold)
                    .frame(width: 280, height: 60)
            }
            .buttonStyle(PrimaryButtonStyle())
        }
        .padding()
        .background(RoundedRectangle(cornerRadius: 20).fill(checkerRed.opacity(0.9)))
        .shadow(radius: 10)
    }
    
    // Move history panel
    private var moveHistoryView: some View {
        VStack(alignment: .leading) {
            Text("Move History:")
                .font(.headline)
                .foregroundColor(checkerBeige)
                .padding(.bottom, 5)
            
            ScrollView {
                VStack(alignment: .leading) {
                    ForEach(moveHistory.reversed(), id: \.id) { entry in
                        HStack {
                            Text(entry.move)
                            Spacer()
                            Text(entry.status)
                                .foregroundColor(entry.isValid ? .green : .red)
                        }
                        .padding()
                        .background(Color.white.opacity(0.1))
                        .cornerRadius(8)
                    }
                }
            }
            .frame(height: 100)
        }
        .padding()
    }
    
    // Winner announcement view
    private func winnerView(winner: String) -> some View {
        VStack {
            Text("🎉 Winner 🎉")
                .font(.title)
                .bold()
                .foregroundColor(checkerBeige)
                .padding()
            
            Text(winner)
                .font(.title2)
                .foregroundColor(.white)
                .padding()
                .background(checkerRed)
                .cornerRadius(10)
        }
        .transition(.scale)
    }
    
    // MARK: - Core Game Logic
    
    // Determines if voice input is available
    private var canUseSpeech: Bool {
        return (speechAuthorizationStatus == .authorized) && micPermissionGranted
    }
    
    // Initializes new game state
    func startGame() {
        gameStarted = true
        currentPlayer = "Player One"
        lastMove = ""
        winner = nil
        moveHistory.removeAll()
        
        if useTestBench {
            TestBench.shared.activate(scenario: testScenario)
        }
    }
    
    // Ends current game and resets state
    func endGame() {
        if useTestBench {
            winner = ["Player One", "Player Two"].randomElement()!
        } else {
            let dataToSend: [String: Any] = ["move": "*"]
            sendDataToServer(data: dataToSend)
        }
        
        gameStarted = false
        moveHistory.removeAll()
        TestBench.shared.deactivate()
    }
    
    // Clears current move input
    func clearLastMove() {
        lastMove = ""
    }
    
    // Processes and sends move to server/test bench
    func sendMove() {
        speechRecognizer.stopListening()
        isListening = false
        
        // Format move for transmission
        let cleanedMove = lastMove.replacingOccurrences(of: " to ", with: "")
        let moveEntry = MoveEntry(
            move: lastMove,
            status: useTestBench ? "Test Mode: Processing..." : "Pending validation...",
            isValid: true
        )
        moveHistory.append(moveEntry)
        lastMove = ""
        
        let dataToSend: [String: Any] = ["move": cleanedMove]
        sendDataToServer(data: dataToSend)
    }
    
    // MARK: - Speech Handling
    
    // Toggles voice input listening state
    func toggleListening() {
        guard canUseSpeech else { return }
        
        if isListening {
            speechRecognizer.stopListening()
            isListening = false
        } else {
            startNewListeningSession()
        }
    }
    
    // Starts new voice recognition session
    func startNewListeningSession() {
        guard canUseSpeech else { return }
        
        speechRecognizer.stopListening()
        speechRecognizer.startListening { [self] result in
            // Handle voice commands
            if result == "CLEAR" {
                DispatchQueue.main.async {
                    self.clearLastMove()
                }
            } else if result == "SEND" {
                DispatchQueue.main.async {
                    self.sendMove()
                }
            } else if result == "RESTART" {
                DispatchQueue.main.async {
                    self.endGame()
                }
            } else {
                // Process numeric move input
                let filteredMove = result.filter { $0.isNumber }.prefix(4)
                if filteredMove.count == 4 {
                    let formattedMove = "\(filteredMove.prefix(2)) to \(filteredMove.suffix(2))"
                    DispatchQueue.main.async {
                        self.lastMove = formattedMove
                    }
                }
            }
        }
        isListening = true
    }
    
    // MARK: - Permissions
    
    // Requests necessary user permissions
    func requestPermissions() {
        // Speech recognition authorization
        SFSpeechRecognizer.requestAuthorization { status in
            DispatchQueue.main.async {
                self.speechAuthorizationStatus = status
            }
        }
        
        // Microphone access
        AVAudioSession.sharedInstance().requestRecordPermission { granted in
            DispatchQueue.main.async {
                self.micPermissionGranted = granted
            }
        }
    }
    
    // MARK: - Networking
    
    // Sends move data to server or test bench
    func sendDataToServer(data: [String: Any]) {
        if useTestBench {
            handleTestResponse(data: data)
            return
        }
        
        guard let url = URL(string: "http://\(serverIP):5000/send_data") else { return }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        
        do {
            let jsonData = try JSONSerialization.data(withJSONObject: data, options: [])
            request.httpBody = jsonData
        } catch {
            updateMoveHistory(status: "Invalid move format", isValid: false)
            return
        }
        
        // Handle server response
        let task = URLSession.shared.dataTask(with: request) { [self] data, response, error in
            if let error = error {
                updateMoveHistory(status: "Connection error: \(error.localizedDescription)", isValid: false)
                return
            }
            
            guard let data = data else {
                updateMoveHistory(status: "No response from server", isValid: false)
                return
            }
            
            do {
                if let jsonResponse = try JSONSerialization.jsonObject(with: data, options: []) as? [String: Any] {
                    let status = jsonResponse["status"] as? Bool ?? false
                    let message = jsonResponse["message"] as? String ?? "Unknown response"
                    
                    DispatchQueue.main.async {
                        if status {
                            if message.contains("DOUBLE_JUMP") {
                                updateMoveHistory(status: message, isValid: true)
                            } else {
                                currentPlayer = (currentPlayer == "Player One") ? "Player Two" : "Player One"
                                updateMoveHistory(status: message, isValid: true)
                            }
                        } else {
                            updateMoveHistory(status: message, isValid: false)
                        }
                    }
                }
            } catch {
                updateMoveHistory(status: "Invalid server response", isValid: false)
            }
        }
        task.resume()
    }
    
    // Handles test bench responses
    private func handleTestResponse(data: [String: Any]) {
        let mockResponse = TestBench.shared.getMockResponse(for: data["move"] as? String ?? "")
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            if mockResponse.status {
                if self.testScenario == .doubleJump {
                    self.updateMoveHistory(status: mockResponse.message, isValid: true)
                } else {
                    self.currentPlayer = (self.currentPlayer == "Player One") ? "Player Two" : "Player One"
                    self.updateMoveHistory(status: mockResponse.message, isValid: true)
                }
            } else {
                self.updateMoveHistory(status: mockResponse.message, isValid: false)
            }
        }
    }
    
    /// Updates move history with validation results
    private func updateMoveHistory(status: String, isValid: Bool) {
        if let lastIndex = moveHistory.indices.last {
            moveHistory[lastIndex].status = status
            moveHistory[lastIndex].isValid = isValid
        }
    }
}

// MARK: - Supporting Structures

// Tracks move history entries
struct MoveEntry: Identifiable {
    let id = UUID()
    var move: String    // Move notation (e.g., "11 to 22")
    var status: String  // Server response status
    var isValid: Bool   // Whether move was valid
}

// Checkerboard game board view
struct CheckerboardView: View {
    let rows = 8
    let columns = 8
    let squareSize: CGFloat = 35
    private let checkerRed = Color(red: 0.76, green: 0.16, blue: 0.19)
    private let checkerBeige = Color(red: 0.98, green: 0.96, blue: 0.84)
    
    var body: some View {
        VStack(spacing: 0) {
            ForEach(0..<rows, id: \.self) { row in
                HStack(spacing: 0) {
                    ForEach(0..<columns, id: \.self) { column in
                        ZStack {
                            // Board square
                            Rectangle()
                                .fill((row + column).isMultiple(of: 2) ? checkerBeige : checkerRed)
                                .frame(width: squareSize, height: squareSize)
                            
                            // Checker piece
                            if shouldShowChecker(row: row, column: column) {
                                Circle()
                                    .fill(checkerColor(for: row))
                                    .frame(width: squareSize * 0.8, height: squareSize * 0.8)
                                    .shadow(radius: 2)
                            }
                        }
                    }
                }
            }
        }
        .background(checkerRed)
        .cornerRadius(4)
        .overlay(
            RoundedRectangle(cornerRadius: 4)
                .stroke(Color.black, lineWidth: 2)
        )
    }
    
    // Determines if checker should be shown at position
    private func shouldShowChecker(row: Int, column: Int) -> Bool {
        let isDarkSquare = (row + column) % 2 == 1
        return isDarkSquare && (row < 3 || row > 4)
    }
    
    // Returns checker color based on row position
    private func checkerColor(for row: Int) -> Color {
        row < 3 ? .black : .white // Black for top rows, white for bottom
    }
}

// MARK: - Button Styles

// Primary button style (used for main actions)
struct PrimaryButtonStyle: ButtonStyle {
    var isEnabled: Bool = true
    private let checkerRed = Color(red: 0.76, green: 0.16, blue: 0.19)
    
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .font(.system(.body, weight: .semibold))
            .foregroundColor(.white)
            .padding(.vertical, 12)
            .padding(.horizontal, 20)
            .background(isEnabled ? checkerRed : Color.gray)
            .cornerRadius(25)
            .scaleEffect(configuration.isPressed ? 0.95 : 1.0)
            .animation(.easeOut(duration: 0.1), value: configuration.isPressed)
            .shadow(color: .black.opacity(0.1), radius: 4, x: 0, y: 2)
    }
}

// Secondary button style (used for auxiliary actions)
struct SecondaryButtonStyle: ButtonStyle {
    private let checkerRed = Color(red: 0.76, green: 0.16, blue: 0.19)
    
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .font(.system(.callout))
            .foregroundColor(.white)
            .padding(.vertical, 10)
            .padding(.horizontal, 15)
            .background(checkerRed)
            .cornerRadius(15)
            .overlay(
                RoundedRectangle(cornerRadius: 15)
                    .stroke(Color.white, lineWidth: 2)
            )
            .scaleEffect(configuration.isPressed ? 0.95 : 1.0)
            .animation(.easeOut(duration: 0.1), value: configuration.isPressed)
    }
}

// MARK: - Preview
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
