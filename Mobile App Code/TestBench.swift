import Foundation

public class TestBench {
    public static let shared = TestBench()
    
    public enum TestScenario: String, CaseIterable {
        case validMove = "Valid Move"
        case invalidMove = "Invalid Move"
        case doubleJump = "Double Jump"
        case connectionError = "Connection Error"
    }
    
    private(set) var isActive = false
    private var currentScenario: TestScenario = .validMove
    
    public struct MockResponse {
        let status: Bool
        let message: String
    }
    
    public func activate(scenario: TestScenario) {
        isActive = true
        currentScenario = scenario
    }
    
    public func deactivate() {
        isActive = false
    }
    
    public func getMockResponse(for move: String) -> MockResponse {
        switch currentScenario {
        case .validMove:
            return MockResponse(
                status: true,
                message: "Valid move: \(move)"
            )
        case .invalidMove:
            return MockResponse(
                status: false,
                message: "Invalid move: \(move)"
            )
        case .doubleJump:
            return MockResponse(
                status: true,
                message: "Double jump possible at \(move)"
            )
        case .connectionError:
            return MockResponse(
                status: false,
                message: "Connection failed for \(move)"
            )
        }
    }
}
